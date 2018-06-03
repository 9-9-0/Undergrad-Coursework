var rp = require('request-promise')

var contract = require('truffle-contract')
var OracleContract = require('./build/contracts/betSysOracle.json')

var Web3 = require('web3')
var web3 = new Web3(new Web3.providers.HttpProvider('http://localhost:7545'))
var config = require('./config.json')

var oracleContract = contract(OracleContract);
oracleContract.setProvider(web3.currentProvider);

if (typeof oracleContract.currentProvider.sendAsync !== "function") {
  oracleContract.currentProvider.sendAsync = function() {
    return oracleContract.currentProvider.send.apply(
      oracleContract.currentProvider, arguments
    );
  };
}

var betSysOracleGlobal;
var watchedGames = {};

//Periodically refresh games (assumed all gameIds are valid), scalability issues with growing list of watchedIds is currently ignored
function getGameInfo(gameId) {
  var options = {
    method : 'GET',
    uri : config.APIUrls.getGame + gameId
  }

  console.log("CALLING " + gameId);
  rp(options)
    .then(res => {
      const gameJSON = JSON.parse(res);
      console.log(gameJSON);

      betSysOracleGlobal.initGame(event.args._betContract, gameJSON._id,
        gameJSON.GameInfo.Sport, gameJSON.GameInfo.HomeTeam, gameJSON.GameInfo.VisitingTeam, {from: accounts[1], gas: 1000000});

      if (gameJSON.GameState.State == 2) {
        //Game has ended, watching is no longer necessary
        delete watchedGames[gameId];
      }
      console.log("HI");
      console.log(watchedGames[gameId]);
    })
    .catch(err => {
      delete watchedGames[gameId];
    })
};

function refreshGames(games) {
  for (var game in watchedGames) {
    if (watchedGames[game] == true) {
      getGameInfo(game);
    }
  }
}

//setInterval(getGameInfo, 300000); //5 minute interval
//setInterval(refreshGames, 2000);

web3.eth.getAccounts((err, accounts) => {
  oracleContract.deployed()
    .then((oracleInstance) => {
      betSysOracleGlobal = oracleInstance;
      events = oracleInstance.allEvents({fromBlock: 0, toBlock: 'latest'});

      events.watch((err, event) => {
        //console.log(event);

        if (event.event == "callbackUpdateGame") {
          var gameId = web3.utils.toUtf8(event.args._gameId);
          console.log(gameId);
          var options = {
            method : 'GET',
            uri : config.APIUrls.getGame + gameId
          };

          rp(options)
            .then(res => {
              const gameJSON = JSON.parse(res);
              console.log(gameJSON);
              //console.log(event.args._betContract);
              //console.log(gameId);

              oracleInstance.initGame(event.args._betContract, gameJSON._id,
                gameJSON.GameInfo.Sport, gameJSON.GameInfo.HomeTeam, gameJSON.GameInfo.VisitingTeam, {from: accounts[1], gas: 1000000});

              //Add to list of games to periodically update
              if (gameJSON.GameState.State != 2) {
                watchedGames[gameId] = true;
                console.log(watchedGames[gameId]);
              }
            })
            .catch(err => {
              console.log("Request for initialization from: " + event.args._betContract);
              console.log("for gameId: " + web3.utils.toUtf8(event.args._gameId));
              console.log("Id was not found. Failing game.")
              oracleInstance.failGame(event.args._betContract, {from: accounts[1]});
            })
        }
        else if (event.event == "callbackUpdateScore") {

        }
      })
  })
  .catch(err => {
    console.log(err);
  })
})
