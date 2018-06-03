pragma solidity ^0.4.21;

contract betSysOracle {
  address private creator;

  enum gameStates { Invalid, Registered, InPlay, Ended }

  struct scores {
    //Home team scores reside in index 0, visiting team scores reside in index 1
    uint [] finalScores;
    uint [2][] partialScores;
    //partial scores, e.g. quarterly, not currently implemented
    //Care for gas consumption when accessing this as it grows
  }

  struct game {
    bytes32 gameId;
    gameStates gameState;
    bytes32 sportId;
    bytes32 homeTeamId;
    bytes32 visitTeamId;
    scores gameScores;
  }

  mapping (address => game) public gameMap;

  modifier isOnly(address _address)  {
    require(msg.sender == _address);
    _;
  }

  constructor() public  {
    creator = msg.sender;
  }

  event gameUpdated(address _betContract); //To notify the betSys contract of the result of gameId initiation.
  event gameInitiated(address _betContract);

  function updateGame(address _betContract, bytes32 _gameId) public
  {
    emit callbackUpdateGame(_betContract, _gameId);
  }

  function updateScore(address _betContract) public
  {
    require(gameMap[_betContract].gameState == gameStates.InPlay);
  }

  //On a successful game lookup, the initial game data is set (these values should never change).
  function initGame(address _betContract, bytes32 _gameId, int _gameState, bytes32 _sportId, bytes32 _homeTeamId, bytes32 _visitTeamId)
    public
    isOnly(creator)
  {
    gameMap[_betContract].gameId = _gameId;
    gameMap[_betContract].gameState = _gameState;
    gameMap[_betContract].sportId = _sportId;
    gameMap[_betContract].homeTeamId = _homeTeamId;
    gameMap[_betContract].visitTeamId = _visitTeamId;
    emit gameInitiated(_betContract);
  }

  //On an unssucessful game lookup or invalid game _currentState, the game is invalidated for the requesting betSys contract and the event is emitted.
  function failGame(address _betContract)
    public
    isOnly(creator)
  {
    gameMap[_betContract].gameState = gameStates.Invalid;
    emit gameInitiated(_betContract);
  }

  //To be used after a game is initiated and valid
  function setGame(address _betContract, int _gameState, int homeScore, int visitScore)
    public
    isOnly(creator)
  {
    if (homeScore != -1 && visitScore != -1) {
      gameMap[_betContract].finalScores[0] = homeScore;
      gameMap[_betContract].finalScores[1] = visitScore;
    }

    gameMap[_betContract].gameState = _gameState;
  }

  //Enum state getter for betSys.sol
  function getState(address _betContract) public constant returns(gameStates) {
    return gameMap[_betContract].gameState;
  }


  /**********************FOR TRUFFLE DEBUGGING************************************/
  function getGameId(address _betContract) public constant returns(bytes32) {
    return gameMap[_betContract].gameId;
  }
  function getSportId(address _betContract) public constant returns(bytes32) {
    return gameMap[_betContract].sportId;
  }
  function getHomeId(address _betContract) public constant returns(bytes32) {
    return gameMap[_betContract].homeTeamId;
  }
  function getVisitId(address _betContract) public constant returns(bytes32) {
    return gameMap[_betContract].visitTeamId;
  }

}
