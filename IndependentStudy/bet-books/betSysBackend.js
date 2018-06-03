var contract = require('truffle-contract')
var BetSysContract = require('./build/contracts/betSys.json')
var OracleContract = require('./build/contracts/betSysOracle.json')

var config = require('./config.json')

var Web3 = require('web3')
var web3 = new Web3(new Web3.providers.HttpProvider('http://localhost:7545'))

var betSysContract = contract(BetSysContract);
betSysContract.setProvider(web3.currentProvider);
var oracleContract = contract(OracleContract);
oracleContract.setProvider(web3.currentProvider);

if (typeof betSysContract.currentProvider.sendAsync !== "function") {
  betSysContract.currentProvider.sendAsync = function() {
    return betSysContract.currentProvider.send.apply(
      betSysContract.currentProvider, arguments
    );
  };
}

if (typeof oracleContract.currentProvider.sendAsync !== "function") {
  oracleContract.currentProvider.sendAsync = function() {
    return oracleContract.currentProvider.send.apply(
      oracleContract.currentProvider, arguments
    );
  };
}

var betSysGlobal; //Figure out a better way to do this?

//Watch for betSys events
web3.eth.getAccounts((err, accounts) => {
  betSysContract.deployed()
    .then((instance) => {
      betSysGlobal = instance;
      events = instance.allEvents({fromBlock: 0, toBlock: 'latest'});

      events.watch((err, event) => {
        console.log(event);

        if (event.event == "checkForValidity") {
          console.log(event);
        }
      })
    })
    .catch((err) => {
      console.log(err);
    })
})

//Watch for oracle events
web3.eth.getAccounts((err, accounts) => {
  oracleContract.deployed()
    .then((instance) => {
      events = instance.allEvents({fromBlock: 0, toBlock: 'latest'});

      events.watch((err, event) => {
        if (event.event == "gameInitiated") {
          betSysGlobal.setValidity({from: accounts[0]});
          console.log("Validity set.");
        }
        if (event.event == "gameUpdated") {
          
        }
      })
    })
    .catch((err) => {
      console.log(err);
    })
})
