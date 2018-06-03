pragma solidity ^0.4.21;

//Interface for our oracle.
contract betSysOracle {
  function updateGame(address _betContract, bytes32 _gameId) public {}
  function getState(address _betContract) public constant returns(int) {}
}

contract betSys {
    enum BetStates { Created, Validating, Invalid, Ready, Open, Closed, Withdrawable }
    //betSysOracle Game States: 0 - Invalid, 1 - Registered, 2 - InPlay, 3 - Ended
    BetStates currentState;
    address private owner;

    bytes32 gameId;
    betSysOracle oracle;
    uint private maxBets; //Limited sizing for now to reduce costs
    int line; //Average of the bets across the range of scores

    struct bet {
      int score; //Ranging from [-50, 50], e.g. 10 = home team wins by 10.
      uint amount; //For future use if variable amounts of ETH are bet.
      bool validity;
    }

    mapping(address => bet[]) userBets;

    modifier isOnly(address _address) {
        require(msg.sender == owner);
        _;
    }

    modifier isState(BetStates _currentState) {
        require(currentState == _currentState);
        _;
    }

    constructor(address _oracleAddress, bytes32 _gameId, uint _maxBets)
      public
    {
      owner = msg.sender;
      oracle = betSysOracle(_oracleAddress); //Need to check for: address is to a contract and of type betSysOracle
      maxBets = _maxBets;
      gameId = _gameId;

      currentState = BetStates.Created;
    }

    //Address that calls this function should ideally be paying for all transaction fees, check that this is indeed the case.
    function requestUpdate()
      public
    {
      oracle.updateGame(address(this), gameId);
    }

    //Calls on the oracle to update game state. Once the oracle event is emitted, the next function is called to set state.
    function validateGame()
      public
      isOnly(owner)
      isState(BetStates.Created)
    {
      requestUpdate();
      currentState = BetStates.Validating;
    }

    //After gameInitiated oracle event is emitted, this is called to validate or invalidate the entire contract.
    function setValidity()
      public
      isOnly(owner)
      isState(BetStates.Validating)
    {
      if (oracle.getState(address(this)) == 1) {
        currentState = BetStates.Ready;
      }
      else {
        currentState = BetStates.Invalid;
      }
    }

    //After the registered game transitions from Registered to InPlay, this should be called to open bets.
    //Call requestUpdate() before calling this.
    function openBets()
      public
      isOnly(owner)
      isState(BetStates.Ready)
    {
      if (oracle.getState(address(this)) == 2) {
        currentState = BetStates.Open;
      }
      else {
        currentState = BetStates.Invalid;
      }
    }

    //Score value based on home/visiting team should be handled by UI
    function requestBet(int score)
      public
      isState(BetStates.Open)
    {
      requestUpdate();
    }


    /*
    function closeBets()
        public
        isState(BetStates.BetsOpen)
    {

    }

    function withdrawFunds() public
    {

    }
    */

    /**********************FOR TRUFFLE DEBUGGING************************************/
    function getValidity() returns(BetStates) {
      return currentState;
    }
}
