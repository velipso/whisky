//
// Whisky - a collection of stateless RNGs
// by Sean Connelly (@velipso), https://sean.cm
// The Unlicense License
// Project Home: https://github.com/velipso/whisky
//

const {readdirSync, readFileSync, writeFileSync, unlinkSync} = require('fs');
const {spawn, spawnSync} = require('child_process');
const {createHash} = require('crypto')
const temp = require('path').join(require('os').tmpdir(), `tmp.${Math.random()}`);

// info about the dieharder tests, sorted so faster tests are first
const testInfo = [
  {id:   8, size:  1, suspect: 0, doNotUse: 0},
  {id:  10, size:  1, suspect: 0, doNotUse: 0},
  {id:  11, size:  1, suspect: 0, doNotUse: 0},
  {id:  15, size:  1, suspect: 0, doNotUse: 0},
  {id: 100, size:  1, suspect: 0, doNotUse: 0},
  {id:   0, size:  1, suspect: 0, doNotUse: 0},
  {id: 206, size:  1, suspect: 0, doNotUse: 0},
  {id: 204, size:  1, suspect: 0, doNotUse: 0},
  {id: 208, size:  2, suspect: 0, doNotUse: 0},
  {id:  12, size:  1, suspect: 0, doNotUse: 0},
  {id:   4, size:  1, suspect: 0, doNotUse: 0},
  {id:   3, size:  1, suspect: 0, doNotUse: 0},
  {id: 209, size:  1, suspect: 0, doNotUse: 0},
  {id: 203, size:  1, suspect: 0, doNotUse: 0},
  {id: 202, size:  1, suspect: 0, doNotUse: 0},
  {id:   9, size:  1, suspect: 0, doNotUse: 0},
  {id:   1, size:  1, suspect: 0, doNotUse: 0},
  {id:  16, size:  1, suspect: 0, doNotUse: 0},
  {id: 101, size:  1, suspect: 0, doNotUse: 0},
  {id: 205, size:  1, suspect: 0, doNotUse: 0},
  {id: 207, size:  1, suspect: 0, doNotUse: 0},
  {id:  13, size:  1, suspect: 0, doNotUse: 0},
  {id: 102, size: 16, suspect: 0, doNotUse: 0},
  {id:   2, size:  1, suspect: 0, doNotUse: 0},
  {id:  -1, size: 96, suspect: 3, doNotUse: 1} // last row is ALL tests, ~1hr to run per axis
];
// removed 5, 6, and 7 because they are marked as "Suspect"
// removed 14 because it's marked as "Do Not Use"
// removed 17 and 201 because they are stupid slow
// removed 200 because it needs extra params (?)
// takes roughly 3min to make it through all tests (except last one) per axis
// last row runs all tests, unfortunately that includes tests marked "Suspect" and "Do Not Use"

function usage(exit, subcommand){
  console.error(
`Usage:
  node whisky.js <subcommand> [parameters...]

Depends on:
  dieharder (requires v3.31.1)
  clang     (tested with v12.0.0)
  node      (tested with v15.4.0)

Subcommands:
  step    Perform a single step of analysis in search of functions
  print   Print a function's source code to console
  score   Rank functions given the current state on disk
${subcommand && subcommand !== 'step' ? '' : `
* step <total> <process> <dimensions> <quality>
       [-f] [-w <weak>] [-s <skip...>]

  Calculates the next step needed to analyze the space of functions,
  and executes dieharder to gather the information.  Exits with 1 if
  no more steps are needed.

  Example:
    node whisky.js step 100 a 2 12

  Where:
    <total>       How many functions to search through
    <process>     Unique identifier when running multiple processes
    <dimensions>  How many inputs to the hash function
    <quality>     How many operations allowed in the hash
    -f            Reject functions that fail any test
    -w <weak>     Reject functions that exceed <weak> weak results
    -s <skip...>  List of indicies to skip (ex: 10502,11523)
`}${subcommand && subcommand !== 'print' ? '' : `
* print <process> <dimensions> <quality> <index>

  Prints the hash function.

  Example:
    node whisky.js print a 2 12 42

  Where:
    <process>     Unique identifier when running multiple processes
    <dimensions>  How many inputs to the hash function
    <quality>     How many operations allowed in the hash
    <index>       Unique identifier for an individual function
`}${subcommand && subcommand !== 'score' ? '' : `
* score [<process>] <dimensions> <quality> [<index>]
        [-i|-f] [-s] [-n] [-c] [-l <limit>]

  Given the current knowledge, calculates the min/max score of the
  selected functions.

  When all tests have ran, a function has a definite score.  But if
  there are still tests left to run, functions are scored as a range
  instead, as lowest possible score <-> highest possible score.

  Note: Lower score is better!

  Example:
    node whisky.js score 2 12

  Where:
    <process>     Unique identifier when running multiple processes
    <dimensions>  How many inputs to the hash function
    <quality>     How many operations allowed in the hash
    <index>       Unique identifier for an individual function
    -i            Sort by min score instead of max score
    -f            Sort by failure rate instead of max score
    -s            Only show summary table
    -n            Normalize scores between 0-1000
    -c            Show code along with rankings
    -l <limit>    Only use the first <limit> functions
`}`);
  process.exit(exit);
}

async function stepMain(args){
  const allowFailures = !argFlag(args, '-f');
  const skipEmail = argFlag(args, '-e');

  let limitWeak = false;
  if (args.includes('-w')){
    const p = args.splice(args.indexOf('-w'), 2);
    limitWeak = parseFloat(p[1]);
    if (isNaN(limitWeak))
      return usage(1, 'step');
  }

  let skipIndex = [];
  if (args.includes('-s')){
    const p = args.splice(args.indexOf('-s'), 2);
    skipIndex = p[1].split(',').map(parseFloat);
    if (skipIndex.some(isNaN))
      return usage(1, 'step');
  }

  if (args.length !== 4)
    return usage(1, 'step');

  const total = parseFloat(args.shift());
  const proc = args.shift();
  const dim = parseFloat(args.shift());
  const quality = parseFloat(args.shift());

  if (isNaN(total) || isNaN(dim) || isNaN(quality))
    return usage(1, 'step');

  // if lots of rngs qualify for the last test, trim the possibility space
  const maxFinished = total <= 10 ? 2 : total <= 50 ? 4 : total <= 100 ? 6 : 8;

  const startTime = Date.now();
  const exit = x => {
    let hrs = Math.round((Date.now() - startTime) / 100);
    const ds = hrs % 10; hrs = Math.floor(hrs / 10);
    const sec = hrs % 60; hrs = Math.floor(hrs / 60);
    const min = hrs % 60; hrs = Math.floor(hrs / 60);
    console.log((
      (hrs > 0 ? `${hrs}hr ` : '') +
      (min > 0 ? `${min}min ` : '') +
      `${sec}.${ds}sec` +
      (min > 5 ? ` at ${(new Date()).toLocaleString()}` : '')
    ).cyan);
    process.exit(x);
  };

  const stateFile = `state.${proc}.${dim}.${quality}.json`;
  let state = {gen: [], ignore: {}};
  try { state = JSON.parse(readFileSync(stateFile, 'utf8')); } catch (e) {}

  while (state.gen.length < total)
    state.gen.push({});

  const scores = state.gen.slice(0, total)
    .map((a, i) => scoreGen(proc, dim, quality, a, i))
    .filter(a => state.ignore[a.i] === undefined && !skipIndex.includes(a.i))
    .filter(a => !(
      (!allowFailures && a.fail > 0) ||
      (limitWeak !== false && a.weak > limitWeak)
    ))
    .sort(sortBy('maxScore', 'i'));

  const ceiling = scores.length > 0
    ? (scores.length > 1 ? scores[1].maxScore : scores[0].maxScore)
    : 0;

  const below = scores.filter(sc => sc.minScore <= ceiling);
  const belowFinished = below.filter(sc => sc.nextTest === false);
  let belowOpen = below.filter(sc => sc.nextTest !== false);

  // if we have a lot of final tests to perform, only perform them on the top X, sorted by minScore
  if (belowOpen.every(sc => isLastTest(sc.nextTest) && sc.nextAxis === 0) &&
    belowOpen.length > maxFinished - belowFinished.length){
    if (belowFinished.length < maxFinished){
      console.log(`Moving forward with best ${maxFinished} of ${below.length} qualified rngs`);
      belowOpen.sort(sortBy('minScore', 'i'));
      belowOpen = belowOpen.slice(0, maxFinished - belowFinished.length);
    }
    else
      belowOpen = [];
  }

  // find the lowest nextTest rng with the highest nextAxis
  const nx = belowOpen.sort(sortBy('nextTest', '^nextAxis', 'minScore', 'i'))[0];

  // any next test required?
  if (!nx){
    const report = [
      `Results for ${proc} ${dim} ${quality}, total ${total}`,
      '(lower score is better)',
      ''
    ];
    const finished = scores.filter(sc => sc.nextTest === false);
    finished.forEach(sc => {
      report.push(`${sc.id} score: ${sc.minScore.toFixed(2)}`);
    });

    if (finished.length > 0){
      report.push(`\n// Best algorithm score: ${finished[0].minScore.toFixed(2)}`);
      report.push(generateRNG(proc, dim, quality, finished[0].i));
    }
    if (finished.length > 1){
      report.push(`\n// Second best algorithm score: ${finished[1].minScore.toFixed(2)}`);
      report.push(generateRNG(proc, dim, quality, finished[1].i));
    }
    if (finished.length <= 0)
      report.push('No RNGs finished!');

    console.log(report.join('\n'));
    // email me the results -- email.js isn't checked into the repo and contains special commands
    // to email me, the author :-)
    // since you don't have the file, the require() throws and nothing happens
    try { if (!skipEmail) require('./email')(report); } catch (e) {}
    exit(1);
    return;
  }

  // run the next test required
  const code = generateRNG(proc, dim, quality, nx.i);
  if (code === false){
    console.error('Failed to generate RNG');
    exit(1);
  }
  if (nx.nextAxis === 0){
    console.log(
      '\n' + nx.idColor,
      '| test',
      `${nx.nextTest}.${nx.nextAxis}.*`.brMagenta, '/',
      `${testInfo.length - 1}.${dim * 2 - 1}.*`.brMagenta, '| rng',
      belowOpen.filter(a => a.nextTest !== nx.nextTest).length + 1, '/', belowOpen.length,
      `\n${nx.minScore.toFixed(2).green} <-> ${nx.maxScore.toFixed(2).green} could be below`,
      ceiling.toFixed(2).green
    );
  }

  const result = state.gen[nx.i];

  const saveState = () => writeFileSync(stateFile, JSON.stringify(state, null, '  '));

  const putIgnore = () => {
    state.ignore[nx.i] = 1;
    saveState();
  };

  const putResult = (testKey, pvalue) => {
    // only overwrite if we have a worse result
    if (typeof result[testKey] === 'number'){
      if (Math.abs(0.5 - pvalue) < Math.abs(0.5 - result[testKey]))
        return false;
      switch (judge(result[testKey])){
        case 'FAIL': nx.fail--; break;
        case 'WEAK': nx.weak--; break;
      }
    }
    result[testKey] = pvalue;
    saveState();
    switch (judge(pvalue)){
      case 'FAIL': nx.fail++; break;
      case 'WEAK': nx.weak++; break;
    }
    return (!allowFailures && nx.fail > 0) ||
      (limitWeak !== false && nx.weak > limitWeak);
  };

  // compile the driver program
  const codeExe = `${temp}.out`;
  writeFileSync(`${temp}.c`, generateC(dim, code));
  const clang = spawnSync('clang', ['-O2', '-o', codeExe, `${temp}.c`], {stdio: 'inherit'}).status;
  unlinkSync(`${temp}.c`);
  if (clang !== 0){
    console.error('Failed to compile');
    exit(1);
  }

  // run the next test across the next axis
  let kill = false;
  await new Promise(resolve => {
    let out = spawn(codeExe, [`${nx.nextAxis}`], {stdio: ['inherit', 'pipe', 'pipe']});
    let die = spawn(
      'dieharder',
      isLastTest(nx.nextTest)
        ? ['-g', '200', '-a']
        : ['-g', '200', '-d', `${testInfo[nx.nextTest].id}`],
      {stdio: [out.stdout, 'pipe', 'pipe']}
    );
    let dieErr = [];
    out.stderr.on('data', d => {
      dieErr = dieErr.concat(d.toString('utf8').trim().split('\n'));
    });
    die.stderr.on('data', d => {
      dieErr = dieErr.concat(d.toString('utf8').trim().split('\n'));
    });
    die.stdout.on('data', d => {
      d.toString('utf8').split('\n').forEach(line => {
        const m = line.match(
          /^\s*(\w+)\|([\s\d]+)\|[\s\d]+\|[\s\d]+\|([\s\d.]+)\|\s*(PASSED|FAILED|WEAK)\s*$/
        );
        if (kill || !m)
          return;

        const testNames = ['diehard_birthdays', 'diehard_operm5', 'diehard_rank_32x32',
          'diehard_rank_6x8', 'diehard_bitstream', 'diehard_opso', 'diehard_oqso',
          'diehard_dna', 'diehard_count_1s_str', 'diehard_count_1s_byt', 'diehard_parking_lot',
          'diehard_2dsphere', 'diehard_3dsphere', 'diehard_squeeze', 'diehard_sums',
          'diehard_runs', 'diehard_craps', 'marsaglia_tsang_gcd', 'sts_monobit', 'sts_runs',
          'sts_serial', 'rgb_bitdist', 'rgb_minimum_distance', 'rgb_permutations',
          'rgb_lagged_sum', 'rgb_kstest_test', 'dab_bytedistrib', 'dab_dct', 'dab_filltree',
          'dab_filltree2', 'dab_monobit2'];

        const testName = m[1].trim();
        const ntup = parseFloat(m[2].trim());
        const pvalue = parseFloat(m[3].trim());
        const testIndex = testNames.indexOf(testName);
        if (testIndex < 0){
          console.error(`Unknown test: ${testName}`);
          exit(1);
        }

        let testBelief = 0;
        if ((['diehard_opso', 'diehard_oqso', 'diehard_dna']).includes(testName))
          testBelief = 1; // suspect
        else if (testName === 'diehard_sums')
          testBelief = 2; // do not use

        const testKey = `${nx.nextTest}.${nx.nextAxis}.${testIndex}.${ntup}.${testBelief}`;
        kill = putResult(testKey, pvalue);

        let str = testKey;
        while (str.length < 14)
          str += ' ';
        console.log(
          nx.idColor,
          isLastTest(nx.nextTest) ? '%' : '|', str.brMagenta, '=>',
          judge(result[testKey]).judgeColor, result[testKey],
          kill ? '(killed)'.brRed : '',
        );

        if (kill){
          die.kill('SIGINT');
          die = false;
        }
      });
    });
    die.on('close', (_, err) => {
      if (dieErr.length > 0){
        console.log(`Ignoring ${nx.id} from now on: ${err}\n${dieErr.join('\n')}`);
        putIgnore();
        kill = true;
      }
      if (out){
        out.kill('SIGINT');
        out = false;
      }
      resolve();
    });
  });
  unlinkSync(codeExe);
  exit(0);
}

function printMain(args){
  if (args.length !== 4)
    return usage(1, 'print');

  const proc = args.shift();
  const dim = parseFloat(args.shift());
  const quality = parseFloat(args.shift());
  const index = parseFloat(args.shift());

  if (isNaN(dim) || isNaN(quality) || isNaN(index))
    return usage(1, 'print');

  const res = generateRNG(proc, dim, quality, index);
  if (res === false){
    console.error(`Failed to generate an RNG with quality ${quality} -- try increasing quality`);
    process.exit(1);
  }
  console.log(res.syntax);
}

function scoreMain(args){
  const sortKey = argFlag(args, '-f') ? 'percFail' : argFlag(args, '-i') ? 'minScore' : 'maxScore';
  const summaryOnly = argFlag(args, '-s');
  const normalize = argFlag(args, '-n');
  const showCode = argFlag(args, '-c');

  let limit = false;
  if (args.includes('-l')){
    const p = args.splice(args.indexOf('-l'), 2);
    limit = parseFloat(p[1]);
    if (isNaN(limit))
      return usage(1, 'score');
  }

  if (args.length < 2 || args.length > 4)
    return usage(1, 'score');

  const proc = args.length > 2 ? args.shift() : false;
  const dim = parseFloat(args.shift());
  const quality = parseFloat(args.shift());
  const index = args.length > 0 ? parseFloat(args.shift()) : false;

  if (isNaN(dim) || isNaN(quality) || isNaN(index))
    return usage(1, 'score');

  const pad = (p, s) => s.length < p ? pad(p, `${s} `) : s;
  const lpad = (p, s) => s.length < p ? lpad(p, ` ${s}`) : s;
  const lpScore = normalize ? 7 : 10;
  const logTable = (list, header) => {
    console.log(`\n${header}\n------------------------------------------------------`);
    let minSum = 0, maxSum = 0, sumTotal = 0;
    const logCode = sc =>
      console.log(
        '  > '.gray +
        generateRNG(sc.proc, sc.dim, sc.quality, sc.i)
          .replace(/\n/g, '\n' + '  > '.gray)
          .syntax,
        '\n'
      );
    list.slice(0, 10).forEach((sc, i) => {
      minSum += sc.minScore;
      maxSum += sc.maxScore;
      sumTotal++;
      console.log(sc.str + (i === 0 ? ' (best)' : i === 1 ? ' (great)' : ' (good)').brGreen);
      if (showCode)
        logCode(sc);
    });
    if (sumTotal){
      minSum /= sumTotal;
      maxSum /= sumTotal;
    }
    const single = minSum.toFixed(2) === maxSum.toFixed(2);
    console.log(
      '...          ' +
      lpad(lpScore, minSum.toFixed(2)).blue +
      (single ? '' :
        ' <-> '.gray + lpad(lpScore, maxSum.toFixed(2)).blue),
      `<= top ${sumTotal} avg`.gray,
      single ? '' : `(mid `.gray + ((minSum + maxSum) * 0.5).toFixed(2).blue + `)`.gray
    );
    list.slice(-4).forEach((sc, i) => {
      console.log(sc.str + (i === 3 ? ' (worst)' : ' (bad)').brRed);
      if (showCode)
        logCode(sc);
    });
  };

  let combined = [];
  let combinedTotalTests = 0;
  readdirSync('.')
    .map(file => {
      const m = file.match(/^state\.([^.])+\.(\d+)\.(\d+)\.json$/);
      if (!m)
        return false;
      return {file, proc: m[1], dim: parseFloat(m[2]), quality: parseFloat(m[3])};
    })
    .filter(f =>
      f &&
      (proc === false || f.proc === proc) &&
      dim === f.dim &&
      quality === f.quality
    )
    .forEach(f => {
      const state = JSON.parse(readFileSync(f.file, 'utf8'));
      if (limit !== false)
        state.gen.splice(limit);

      let totalTests = 0;
      state.gen.forEach(a => totalTests += Object.keys(a).length);
      combinedTotalTests += totalTests;

      const scores = state.gen
        .map((a, i) => scoreGen(f.proc, dim, quality, a, i))
        .filter(a => state.ignore[a.i] === undefined)
        .map((sc, i) => {
          const a = state.gen[sc.i];
          const keys = Object.keys(a);
          sc.numTests = keys.length;
          sc.percFail = keys.length > 0 ? sc.fail * 100 / keys.length : 999;
          return sc;
        })
        .sort(sortBy(sortKey, '^numTests', 'i'));

      if (normalize){
        const maxScoreAll = calcScore({}, dim, [], 0);
        scores.forEach(sc => {
          sc.minScore = sc.minScore * 1000 / maxScoreAll;
          sc.maxScore = sc.maxScore * 1000 / maxScoreAll;
        });
      }

      const genStr = sc => sc.str =
        `${f.proc} ${dim} ${quality} `.gray +
        pad(6, `${sc.i}`).brYellow +
        lpad(lpScore, `${sc.minScore.toFixed(2)}`).green +
        (sc.minScore.toFixed(2) === sc.maxScore.toFixed(2)
          ? ''
          : ` <-> ` + lpad(lpScore, sc.maxScore.toFixed(2)).green) +
        (`${lpad(8, `${sc.fail}`).brRed}/${`${sc.weak}`.brYellow}`);
      scores.forEach(genStr);

      if (index){
        const a = state.gen[index];
        if (!a){
          console.error(`No knowledge on index ${index}`);
          return;
        }
        const keys = Object.keys(a);
        keys.sort((a, b) => {
          a = a.split('.');
          b = b.split('.');
          for (let i = 0; i < a.length; i++){
            const v = parseFloat(a[i]) - parseFloat(b[i]);
            if (v !== 0)
              return v;
          }
          return 0;
        });
        let nFail = 0, nWeak = 0, nPass = 0;
        keys.forEach(key => {
          const j = judge(a[key]);
          switch (j){
            case 'FAIL': nFail++; break;
            case 'WEAK': nWeak++; break;
            case 'pass': nPass++; break;
          }
          console.log(
            pad(15, `${key}`).brMagenta +
            pad(15, `${a[key]}`).green +
            j.judgeColor
          );
        });
        const lp = Math.ceil(Math.log10(keys.length));
        const perc = (a, b) => b === 0 ? '' : lpad(8, `${(a * 100 / b).toFixed(1)}%`);
        console.log(
          '------------------------------------------------------\n' +
          `${lpad(lp, `${nFail}`).green} ${'FAIL'.judgeColor}${perc(nFail, keys.length)} +\n` +
          `${lpad(lp, `${nWeak}`).green} ${'WEAK'.judgeColor}${perc(nWeak, keys.length)} +\n` +
          `${lpad(lp, `${nPass}`).green} ${'pass'.judgeColor}${perc(nPass, keys.length)} =\n` +
          `${lpad(lp, `${keys.length}`).green} total 100.0%\n`
        );
        scores.filter(sc => sc.i === index).forEach(sc => console.log(sc.str));
        console.log(`\n${generateRNG(f.proc, dim, quality, index).syntax}`);
      }
      else{
        combined = combined.concat(scores);
        if (!summaryOnly)
          logTable(scores, `${f.proc} ${dim} ${quality} - ${`${totalTests}`.brBlue} tests`);
      }
    });

  if (!proc){
    combined.sort(sortBy(sortKey, '^numTests', 'i'));
    logTable(combined, `Summary - ${`${combinedTotalTests}`.brBlue} tests`);
  }
}

function sortBy(){
  const keys = [].slice.apply(arguments);
  return (a, b) => {
    for (const k of keys){
      const v = k.startsWith('^') // reverse order?
        ? b[k.substr(1)] - a[k.substr(1)]
        : a[k] - b[k];
      if (v !== 0)
        return v;
    }
    return 0;
  };
}

function argFlag(args, flag){
  const p = args.indexOf(flag);
  if (p < 0)
    return false;
  args.splice(p, 1);
  return true;
}

function isLastTest(testNum){
  return testNum === testInfo.length - 1;
}

function judge(p){
  // ported from dieharder source code
  const Xweak = 0.005;
  const Xfail = 0.000001;
  if (p < Xfail || p > 1 - Xfail)
    return 'FAIL';
  if (p < Xweak || p > 1 - Xweak)
    return 'WEAK';
  return 'pass';
}

function calcScore(a, dim, keys, defaultP){
  let score = 0;
  const addP = (p, belief) => {
    belief = ([1, 2, 8])[belief];
    switch (judge(p)){
      case 'FAIL': score += 700 / belief; break;
      case 'WEAK': score += 100 / belief; break;
      case 'pass': score += Math.abs(0.5 - p) / (100 * belief); break;
    }
  };
  for (let test = 0; test < testInfo.length; test++){
    let belief1 = testInfo[test].suspect * 2 * dim;
    let belief2 = testInfo[test].doNotUse * 2 * dim;
    const sk = keys.filter(k => k.startsWith(`${test}.`));
    sk.forEach(k => {
      const belief = parseFloat(k.split('.')[4]);
      if (belief === 1)
        belief1--;
      if (belief === 2)
        belief2--;
      addP(a[k], belief);
    });
    for (let d = 0; d < testInfo[test].size * dim * 2 - sk.length; d++){
      let belief = 0;
      if (belief1 > 0){
        belief = 1;
        belief1--;
      }
      else if (belief2 > 0){
        belief = 2;
        belief2--;
      }
      addP(defaultP, belief);
    }
  }
  return score;
}

function scoreGen(proc, dim, quality, a, i){
  const id = `${proc} ${dim} ${quality} ${i}`;
  const idColor = `${proc} ${dim} ${quality}`.gray + ` ${i}`.brYellow;
  const keys = Object.keys(a);
  const minScore = calcScore(a, dim, keys, 0.5);
  const maxScore = calcScore(a, dim, keys, 0);
  let nextTest = 0, nextAxis = 0;
  const keysWith = str => keys.filter(k => k.startsWith(str)).length;
  for (; nextTest < testInfo.length; nextTest++){
    if (keysWith(`${nextTest}.`) < testInfo[nextTest].size * dim * 2){
      for (let axis = 0; axis < dim * 2; axis++){
        if (keysWith(`${nextTest}.${axis}.`) < testInfo[nextTest].size){
          nextAxis = axis;
          break;
        }
      }
      break;
    }
  }
  if (nextTest === testInfo.length)
    nextTest = false;
  let fail = 0, weak = 0;
  keys.forEach(k => {
    switch (judge(a[k])){
      case 'FAIL': fail++; break;
      case 'WEAK': weak++; break;
    }
  });
  return {proc, dim, quality, i, id, idColor, minScore, maxScore, nextTest, nextAxis, fail, weak};
}

function generateC(dim, code){
  const p = [];
  for (let d = 0; d < dim; d++)
    p.push(`n[${d}]`);
  return `#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

static inline ${code}

int main(int argc, char **argv){
  if (argc < 2){
    fprintf(stderr, "Missing axis\\n");
    return 1;
  }
  uint32_t n[${dim}] = {0};
  uint32_t v[1000];
  int axis = atoi(argv[1]);
  if (axis < 0 || axis >= ${dim * 2}){
    fprintf(stderr, "Bad axis\\n");
    return 1;
  }
  int dir = (axis & 1) ? -1 : 1;
  axis >>= 1;
  for (int i = 0; i < 1000; i++, n[axis] += dir)
    v[i] = whisky${dim}(${p.join(', ')});
  int allzero = 1;
  for (int i = 0; i < 1000 && allzero; i++)
    allzero = v[i] == 0;
  if (allzero){
    fprintf(stderr, "All zero\\n");
    return 1;
  }
  while (1){
    fwrite(v, sizeof(uint32_t), 1000, stdout);
    for (int i = 0; i < 1000; i++, n[axis] += dir)
      v[i] = whisky${dim}(${p.join(', ')});
  }
  return 0;
}
`;
}

function sha256(data){
  const hash = createHash('sha256');
  hash.update(data);
  return hash.digest('hex');
}

function generateRNG(proc, dim, quality, index){
  // generate a random number between low and high (inclusive)
  let round = 0;
  const rnd = (low, high) => {
    const source = sha256(sha256(
      `r ${round++} p ${proc} d ${dim} q ${quality} i ${index}`
    ));
    const M1 = parseInt(source.substr(0, 8), 16);
    const M2 = parseInt(source.substr(8, 8), 16);
    const view = new DataView(new ArrayBuffer(8));
    view.setInt32(0, (M1 << 20) | (M2 >>> 12), true);
    view.setInt32(4, 0x3FF00000 | (M1 >>> 12), true);
    const v = view.getFloat64(0, true) - 1;
    return low + Math.floor(v * (high - low + 1));
  };

  // generate a random prime with a certain number of bits
  const rndPrime = bits => {
    const isPrime = v => {
      if ((v % 2) == 0 || (v % 3) == 0)
        return false;
      const max = Math.floor(Math.sqrt(v));
      for (let i = 5; i <= max; i += 6){
        if ((v % i) == 0 || (v % (i + 2)) == 0)
          return false;
      }
      return true;
    };
    let s = rnd(Math.pow(2, bits - 1), Math.pow(2, bits) - 1);
    while (!isPrime(s))
      s++;
    return s;
  };

  // get a random element in a list
  const rndGet = a => a[rnd(0, a.length - 1)];

  // get a random element in a list, but favor the tail
  const rndGetTail = a => {
    for (let i = a.length - 1; i >= 1; i--)
      if (rnd(0, 1)) return a[i];
    return a[0];
  };

  // remove a random element from a list
  const rndRemove = a => a.splice(rnd(0, a.length - 1), 1)[0];

  // left pad with space
  const pad = (p, s) => s.length < p ? pad(p, ` ${s}`) : s;

  // make a variable name from a prefix and number
  const varChar = '0123456789abcdefghijklmnopqrstuvwxyz';
  const makeVar = (pfx, n) => n < varChar.length
    ? pfx + varChar.charAt(n)
    : makeVar(pfx, Math.floor(n / varChar.length)) +
      varChar.charAt(n % varChar.length);

  // possible templates for each line, where:
  //   x0,x1  input variables
  //   P      random large prime
  //   S,T    random shift amount, where S+T = 32
  const templates = [{
    cost: 2,
    line: '(x0 * P) ^ x1'
  }, {
    cost: 3,
    lossy: true, // flag x1 as lossy, since bits are thrown away
    line: '(x0 * P) ^ (x1 >> S)'
  }, {
    cost: 4,
    line: '(x0 * P) ^ (x1 >> S) ^ x1'
  }, {
    cost: 5,
    line: '(x0 * P) ^ (x1 >> S) ^ (x1 << T)'
  }];

  const add = (budget, lines, unusedVars, allVars) => {
    // if we have a single result, and no budget, then we're done!
    if (unusedVars.length === 1 && budget === 0)
      return lines;

    // find templates we can afford
    const valid = templates.filter(t => t.cost <= budget);

    // keep trying to apply a random template until we succeed
    while (valid.length > 0){
      const {cost, lossy, line} = rndRemove(valid);
      const budget2 = budget - cost;
      const lines2 = lines.concat();
      const unusedVars2 = unusedVars.concat();
      const allVars2 = allVars.concat();

      // generate our prime and shift amounts
      const P = rndPrime(rnd(28, 32));
      const S = rnd(4, 28)
      const T = 32 - S;

      const x0 = unusedVars2.length > 0
        ? rndRemove(unusedVars2)
        : rndGetTail(allVars);

      // if the template has a lossy x1, don't remove from unused
      const x1 = unusedVars2.length > 0
        ? (lossy ? rndGet(unusedVars2) : rndRemove(unusedVars2))
        : rndGetTail(allVars);

      // generate the line by filling in the template
      lines2.push(
        line
          .replace(/P/g, pad(10, `${P}`))
          .replace(/S/g, pad(2, `${S}`))
          .replace(/T/g, pad(2, `${T}`))
          .replace(/x0/g, x0)
          .replace(/x1/g, x1)
      );

      // add the result variable to our variable lists
      const z = makeVar('z', lines.length);
      allVars2.push(z);
      unusedVars2.push(z);

      // try to add more lines
      const result = add(budget2, lines2, unusedVars2, allVars2);
      if (result)
        return result;
    }

    // no templates can be applied, so fail
    return false;
  };

  // generate our initial state based on dimensions
  const params = [];
  for (let d = 0; d < dim; d++)
    params.push(makeVar('i', d));

  // try to add all the lines
  const lines = add(quality, [], params, params);
  if (!lines)
    return false;

  // generate our final function
  return (
    `uint32_t whisky${dim}( // ${proc} ${dim} ${quality} ${index}\n` +
    `  uint32_t ${params.join(`,\n  uint32_t `)}\n` +
    `){\n` +
    lines.map((line, i) =>
      `  uint32_t ${makeVar('z', i)} = ${line};\n`
    ).join('') +
    `  return ${makeVar('z', lines.length - 1)};\n` +
    `}`
  );
}

// so I might have gone a little overboard with the colors :-)
{
  const codes = {
    red: 31, green: 32, yellow: 33, blue: 34, magenta: 35, cyan: 36, white: 37, gray: 90,
    brRed: 91, brGreen: 92, brYellow: 93, brBlue: 94, brMagenta: 95, brCyan: 96, brWhite: 97
  };
  Object.keys(codes).forEach(key =>
    String.prototype.__defineGetter__(key, function(){
      return `\u001b[${codes[key]}m${this}\u001b[39m`;
    })
  );
}

// simple syntax highlighting
String.prototype.__defineGetter__('syntax', function(){
  return this
    .replace(/\b\d+\)/g, r => `${r.substr(0, r.length - 1).blue})`)
    .replace(/\/\/.*/g, r => r.gray)
    .replace(/uint32_t/g, 'uint32_t'.cyan)
    .replace(/return/g, 'return'.red)
    .replace(/whisky[^(]+/g, r => r.green);
});

String.prototype.__defineGetter__('judgeColor', function(){
  switch (this.trim()){
    case 'FAIL': return this.brRed;
    case 'WEAK': return this.brYellow;
    case 'pass': return this.brBlue;
  }
  return this;
});

//
// Main
//

if (process.argv.length < 3)
  usage(0);
else{
  const args = process.argv.slice(2);
  const subcommand = args.shift();
  switch (subcommand){
    case 'step': stepMain(args); break;
    case 'print': printMain(args); break;
    case 'score': scoreMain(args); break;
    default:
      console.error(`Invalid subcommand: ${subcommand}`);
      usage(1);
  }
}
