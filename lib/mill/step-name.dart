import 'mill-base.dart';
import 'position.dart';

class StepName {
  //
  static translate(Position position, Move step) {
    //
    final side = Side.of(position.pieceAt(step.from));

    final chessName = nameOf(position, step);

    String result = chessName;

    if (step.ty == step.fy) {
      //
      result += ''; // TODO
      //
    } else {
      //
      final direction = (side == Side.White) ? -1 : 1;
      final dir = ((step.ty - step.fy) * direction > 0) ? '进' : '退';

      String targetPos;

      result += '$dir$targetPos';
    }

    return step.stepName = result;
  }

  static nameOf(Position position, Move step) {
    return '';
  }

  static findPieceSameCol(Position position, String piece) {
    //
    final map = Map<int, List<int>>();

    for (var row = 0; row < 10; row++) {
      for (var col = 0; col < 9; col++) {
        //
        if (position.pieceAt(row * 9 + col) == piece) {
          //
          var fyIndexes = map[col] ?? [];
          fyIndexes.add(row);
          map[col] = fyIndexes;
        }
      }
    }

    final Map<int, List<int>> result = {};

    map.forEach((k, v) {
      if (v.length > 1) result[k] = v;
    });

    return result;
  }
}
