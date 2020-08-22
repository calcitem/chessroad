import 'cc-base.dart';
import 'position.dart';
import 'step-enum.dart';
import 'steps-validate.dart';

class ChessRules {
  //
  static checked(Position position) {
    //
    final myKingPos = findKingPos(position);

    final oppoPosition = Position.clone(position);
    oppoPosition.trunSide();

    final oppoSteps = StepsEnumerator.enumSteps(oppoPosition);

    for (var step in oppoSteps) {
      if (step.to == myKingPos) return true;
    }

    return false;
  }

  static willBeChecked(Position position, Move move) {
    //
    final tempPosition = Position.clone(position);
    tempPosition.moveTest(move);

    return checked(tempPosition);
  }

  static willKingsMeeting(Position position, Move move) {
    //
    final tempPosition = Position.clone(position);
    tempPosition.moveTest(move);

    for (var col = 3; col < 6; col++) {
      //
      var foundKingAlready = false;

      for (var row = 0; row < 10; row++) {
        //
        final piece = tempPosition.pieceAt(row * 9 + col);

        if (!foundKingAlready) {
          if (piece == Piece.RedKing || piece == Piece.BlackKing)
            foundKingAlready = true;
          if (row > 2) break;
        } else {
          if (piece == Piece.RedKing || piece == Piece.BlackKing) return true;
          if (piece != Piece.Empty) break;
        }
      }
    }

    return false;
  }

  static bool beKilled(Position position) {
    //
    List<Move> steps = StepsEnumerator.enumSteps(position);

    for (var step in steps) {
      if (StepValidate.validate(position, step)) return false;
    }

    return true;
  }

  static int findKingPos(Position position) {
    //
    for (var i = 0; i < 90; i++) {
      //
      final piece = position.pieceAt(i);

      if (piece == Piece.RedKing || piece == Piece.BlackKing) {
        if (position.side == Side.of(piece)) return i;
      }
    }

    return -1;
  }
}
