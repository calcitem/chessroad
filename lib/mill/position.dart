import '../mill/mill-recorder.dart';

import 'mill-base.dart';
import 'step-name.dart';
import 'steps-validate.dart';

class Position {
  //
  BattleResult result = BattleResult.Pending;

  String _side;
  List<String> _pieces; // 10 行，9 列
  CCRecorder _recorder;

  Position.defaultPosition() {
    initDefaultPosition();
  }

  void initDefaultPosition() {
    //
    _side = Side.White;
    _pieces = List<String>(90);

    for (var i = 0; i < 90; i++) {
      _pieces[i] ??= Piece.Empty;
    }

    _recorder = CCRecorder(lastCapturedPosition: toFen());
  }

  Position.clone(Position other) {
    //
    _pieces = List<String>();

    other._pieces.forEach((piece) => _pieces.add(piece));

    _side = other._side;

    _recorder = other._recorder;
  }

  String move(int from, int to) {
    //
    if (!validateMove(from, to)) return null;

    final captured = _pieces[to];

    final move = Move(from, to, captured: captured);
    StepName.translate(this, move);
    _recorder.stepIn(move, this);

    // 修改棋盘
    _pieces[to] = _pieces[from];
    _pieces[from] = Piece.Empty;

    // 交换走棋方
    _side = Side.oppo(_side);

    return captured;
  }

  // 验证移动棋子的着法是否合法
  bool validateMove(int from, int to) {
    // 移动的棋子的选手，应该是当前方
    if (Side.of(_pieces[from]) != _side) return false;
    return (StepValidate.validate(this, Move(from, to)));
  }

  // 在判断行棋合法性等环节，要在克隆的棋盘上进行行棋假设，然后检查效果
  // 这种情况下不验证、不记录、不翻译
  void moveTest(Move move, {turnSide = false}) {
    //
    // 修改棋盘
    _pieces[move.to] = _pieces[move.from];
    _pieces[move.from] = Piece.Empty;

    // 交换走棋方
    if (turnSide) _side = Side.oppo(_side);
  }

  bool regret() {
    //
    final lastMove = _recorder.removeLast();
    if (lastMove == null) return false;

    _pieces[lastMove.from] = _pieces[lastMove.to];
    _pieces[lastMove.to] = lastMove.captured;

    _side = Side.oppo(_side);

    final counterMarks = CCRecorder.fromCounterMarks(lastMove.counterMarks);
    _recorder.halfMove = counterMarks.halfMove;
    _recorder.fullMove = counterMarks.fullMove;

    if (lastMove.captured != Piece.Empty) {
      //
      // 查找上一个吃子局面（或开局），NativeEngine 需要
      final tempPosition = Position.clone(this);

      final moves = _recorder.reverseMovesToPrevCapture();
      moves.forEach((move) {
        //
        tempPosition._pieces[move.from] = tempPosition._pieces[move.to];
        tempPosition._pieces[move.to] = move.captured;

        tempPosition._side = Side.oppo(tempPosition._side);
      });

      _recorder.lastCapturedPosition = tempPosition.toFen();
    }

    result = BattleResult.Pending;

    return true;
  }

  String toFen() {
    //
    var fen = '';

    for (var row = 0; row < 10; row++) {
      //
      var emptyCounter = 0;

      for (var column = 0; column < 9; column++) {
        //
        final piece = pieceAt(row * 9 + column);

        if (piece == Piece.Empty) {
          //
          emptyCounter++;
          //
        } else {
          //
          if (emptyCounter > 0) {
            fen += emptyCounter.toString();
            emptyCounter = 0;
          }

          fen += piece;
        }
      }

      if (emptyCounter > 0) fen += emptyCounter.toString();

      if (row < 9) fen += '/';
    }

    fen += ' $side';

    // 王车易位和吃过路兵标志
    fen += ' - - ';

    // step counter
    fen += '${_recorder?.halfMove ?? 0} ${_recorder?.fullMove ?? 0}';

    return fen;
  }

  String movesSinceLastCaptured() {
    //
    var steps = '', posAfterLastCaptured = 0;

    for (var i = _recorder.stepsCount - 1; i >= 0; i--) {
      if (_recorder.stepAt(i).captured != Piece.Empty) break;
      posAfterLastCaptured = i;
    }

    for (var i = posAfterLastCaptured; i < _recorder.stepsCount; i++) {
      steps += ' ${_recorder.stepAt(i).step}';
    }

    return steps.length > 0 ? steps.substring(1) : '';
  }

  get manualText => _recorder.buildManualText();

  get side => _side;

  trunSide() => _side = Side.oppo(_side);

  String pieceAt(int index) => _pieces[index];

  get halfMove => _recorder.halfMove;

  get fullMove => _recorder.fullMove;

  get lastMove => _recorder.last;

  get lastCapturedPosition => _recorder.lastCapturedPosition;
}
