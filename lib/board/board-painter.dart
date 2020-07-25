import 'dart:math';

import 'package:flutter/material.dart';
import '../board/painter-base.dart';
import '../common/color-consts.dart';
import 'board-widget.dart';

class BoardPainter extends PainterBase {
  //
  BoardPainter({@required double width}) : super(width: width);

  @override
  void paint(Canvas canvas, Size size) {
    //
    doPaint(
      canvas,
      thePaint,
      gridWidth,
      squareSide,
      offsetX: BoardWidget.Padding + squareSide / 2,
      offsetY: BoardWidget.Padding + BoardWidget.DigitsHeight + squareSide / 2,
    );
  }

  @override
  bool shouldRepaint(CustomPainter oldDelegate) {
    return false;
  }

  static doPaint(
    Canvas canvas,
    Paint paint,
    double gridWidth,
    double squareSide, {
    double offsetX,
    double offsetY,
  }) {
    //
    paint.color = ColorConsts.BoardLine;
    paint.style = PaintingStyle.stroke;

    var left = offsetX, top = offsetY;

    // 外框
    paint.strokeWidth = 2;

    canvas.drawRect(
      //Rect.fromLTWH(left, top, gridWidth, squareSide * 6),
      Rect.fromLTWH(left, top, squareSide * 6, squareSide * 6),
      paint,
    );

    paint.strokeWidth = 1;

    // 8 根中间的横线
    for (var i = 1; i < 6; i++) {
      canvas.drawLine(
        Offset(left, top + squareSide * i),
        //Offset(left + gridWidth, top + squareSide * i),
        Offset(left + squareSide * 6, top + squareSide * i),
        paint,
      );
    }

    // 上下各6根短竖线
    for (var i = 0; i < 6; i++) {
      canvas.drawLine(
        Offset(left + squareSide * i, top),
        Offset(left + squareSide * i, top + squareSide * 6),
        paint,
      );
    }

    canvas.drawLine(
      Offset(left + 0, top),
      Offset(left + squareSide * 6, top + squareSide * 6),
      paint,
    );

    canvas.drawLine(
      Offset(left + squareSide * 6, top),
      Offset(left + squareSide * 0, top + squareSide * 6),
      paint,
    );
  }
}
