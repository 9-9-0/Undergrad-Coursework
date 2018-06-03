var betSys = artifacts.require("./betSys.sol");
var betSysOracle = artifacts.require("./betSysOracle.sol");

module.exports = function(deployer, network, accounts) {
	deployer.deploy(betSysOracle, {from: accounts[1]})
		.then(() => {
			console.log("BETSYS ORACLE DEPLOYED");

			/*
			//Deploy an instance of betSys with the desired gameId (this is for the case of a non-existent gameId)
			return deployer.deploy(betSys, betSysOracle.address, 'abc', 100)
				.then(() => {
					console.log("BETSYS DEPLOYED WITH ORACLE ADDRESS: " + betSysOracle.address);
				})
			*/

			//Deploy an instance of betSys with a successful gameId
			//Figure out why sometimes this doesn't get deployed successfully after the oracle is deployed...
			return deployer.deploy(betSys, betSysOracle.address, '5adf6c71fdf4530c7105d5f5', 100)
				.then(() => {
					console.log("BETSYS DEPLOYED WITH ORACLE ADDRESS: " + betSysOracle.address);
				})
		});
};
