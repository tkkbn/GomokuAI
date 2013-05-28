#include "brain.h"

Brain::Brain(StoneType own_stone) {
  this->set_own_stone(own_stone);
}

void Brain::PrintBoard(Board board) {
  char c;

  for (int i = 0; i < kBoardSize; i++) {
    for (int j = 0; j < kBoardSize; j++) {
      StoneType stone = board.stone(kBoardSize - j - 1, i);
      if (stone == kStoneBlack) {
        c = 'x';
      } else if (stone == kStoneWhite) {
        c = 'o';
      } else {
        c = '.';
      }
      printf("%c", c);
    }
    printf("\n");
  }
}

Position Brain::MiniMaxSearch(Board board) {
  return GetSearchPoint(board, kRecursiveNum, false);
}

Position Brain::AlphaBetaPruning(Board board) {
  return GetSearchPoint(board, kRecursiveNum, true);
}

// Original Algorithm
Position Brain::GetPutPoint(Board board) {
  Board::Line line;
  Position put_point;

  if (kAllowedLong[opponent_stone()] == true) {
    for (int i = kBoardSize; i >= 5; i--) {
      line = board.FindAliveDiscontinuousLine(i, opponent_stone());
      if (line.Exists()) {
        put_point = board.GetExtendPoint(line, own_stone());
        if (put_point.Exists() &&
            board.IsBannedPoint(put_point, own_stone()) == false) {
          return put_point;
        }
      }
    }
  }

  line = board.FindAliveDiscontinuousLine(4, own_stone());
  if (line.Exists()) {
    put_point = board.GetExtendPoint(line, own_stone());
    if (put_point.Exists() &&
        board.IsBannedPoint(put_point, own_stone()) == false) {
      return put_point;
    }
  }
  line = board.FindAliveDiscontinuousLine(4, opponent_stone());
  if (line.Exists()) {
    put_point = board.GetExtendPoint(line, own_stone());
    if (put_point.Exists() &&
        board.IsBannedPoint(put_point, own_stone()) == false) {
      return put_point;
    }
  }

  put_point = board.FindMultipleLineMakablePoint(4, 3, own_stone());
  if (put_point.Exists()) {
    return put_point;
  }

  line = board.FindAliveDiscontinuousLine(3, own_stone());
  if (line.Exists()) {
    put_point = board.GetExtendPoint(line, own_stone());
    if (put_point.Exists() &&
        board.IsBannedPoint(put_point, own_stone()) == false) {
      return put_point;
    }
  }

  put_point = board.FindMultipleLinePreMakablePoint(4, own_stone());
  if (put_point.Exists()) {
    return put_point;
  }

  line = board.FindAliveDiscontinuousLine(3, opponent_stone());
  if (line.Exists()) {
    put_point = board.GetExtendPoint(line, own_stone());
    if (put_point.Exists() &&
        board.IsBannedPoint(put_point, own_stone()) == false) {
      return put_point;
    }
  }

  put_point = board.FindMultipleLineMakablePoint(4, 3, opponent_stone());
  if (put_point.Exists()) {
    return put_point;
  }

  put_point = board.FindMultipleLinePreMakablePoint(4, opponent_stone());
  if (put_point.Exists()) {
    return put_point;
  }

  if (kAllowed3x3[own_stone()] == true) {
    put_point = board.FindMultipleLineMakablePoint(3, 3, own_stone());
    if (put_point.Exists()) {
      return put_point;
    }
  }

  put_point = board.FindMultipleLinePreMakablePoint(3, own_stone());
  if (put_point.Exists()) {
    return put_point;
  }

  if (kAllowed3x3[opponent_stone()] == true) {
    put_point = board.FindMultipleLineMakablePoint(3, 3, opponent_stone());
    if (put_point.Exists()) {
      return put_point;
    }
  }

  line = board.FindAliveDiscontinuousLine(2, own_stone());
  if (line.Exists()) {
    put_point = board.GetExtendPoint(line, own_stone());
    if (put_point.Exists() &&
        board.IsBannedPoint(put_point, own_stone()) == false) {
      return put_point;
    }
  }

  put_point = board.FindMultipleLineMakablePoint(2, 2, own_stone());
  if (put_point.Exists()) {
    return put_point;
  }

  return GetEmptyPoint(board);
}

void Brain::set_own_stone(StoneType own_stone) {
  own_stone_ = own_stone;
}

StoneType Brain::own_stone() {
  return own_stone_;
}

StoneType Brain::opponent_stone() {
  return OppositeStone(own_stone_);
}

bool Brain::IsPointToPut(Board board, int x, int y) {
  int search_length = 1;
  if (board.stone(x, y) != kStoneBlank
      || board.IsBannedPoint(x, y, board.StoneToPut()) == true) {
    return false;
  }
  for (int i = x - search_length; i < x + search_length; i++) {
    for (int j = y - search_length; j < y + search_length; j++) {
      if (Board::IsInTheBoard(i, j)
          && board.stone(i, j) != kStoneBlank) {
        return true;
      }
    }
  }
  return false;
}

int Brain::HeuristicCenterHigh(Board board) {
  int num = 0;
  int half_size = kBoardSize / 2;
  for (int i = 0; i < kBoardSize; i++) {
    for (int j = 0; j < kBoardSize; j++) {
      if (board.stone(i, j) == own_stone()) {
        num += half_size - abs(half_size - i);
        num += half_size - abs(half_size - j);
      }
    }
  }
  return num;
}

int Brain::Heuristic(Board board) {
  StoneType own_stone = board.StoneToPut();
  StoneType opponent_stone = OppositeStone(own_stone);
  Board::Line line;
  int heuristic = kHeuristicMin, length;
  int num_of_length[6];

  for (int i = 0; i < 6; i++) {
    num_of_length[i] = 0;
  }

  heuristic += HeuristicCenterHigh(board);

    for (int i = 0; i < kBoardSize; i++) {
      for (int j = 0; j < kBoardSize; j++) {
      std::list<int> length_list =
          board.GetAliveDiscontinuousLineLengthList(i, j, own_stone);
      std::list<int>::iterator list_iter = length_list.begin();
      while (list_iter != length_list.end()) {
        length = *list_iter;
        if (length > 5) {
          length = 5;
        }
        num_of_length[length]++;
        list_iter++;
      }
    }
  }

  for (int i = 0; i < 6; i++) {
    heuristic += num_of_length[i] * pow(10, i);
  }

  for (int i = 0; i < 6; i++) {
    num_of_length[i] = 0;
  }

  for (int i = 0; i < kBoardSize; i++) {
    for (int j = 0; j < kBoardSize; j++) {
      std::list<int> length_list =
          board.GetAliveDiscontinuousLineLengthList(i, j, opponent_stone);
      std::list<int>::iterator list_iter = length_list.begin();
      while (list_iter != length_list.end()) {
        length = *list_iter;
        if (length > 5) {
          length = 5;
        }
        num_of_length[length]++;
        list_iter++;
      }
    }
  }

  for (int i = 0; i < 6; i++) {
    heuristic -= num_of_length[i] * pow(10, i);
  }

  return heuristic;
}

Position Brain::GetSearchPoint(Board board, int count, bool alpha_beta) {
  Position put_point;
  Board virtual_board;
  int max_min_max = 0, current_min_max;

  for (int i = 0; i < kBoardSize; i++) {
    for (int j = 0; j < kBoardSize; j++) {
      if (IsPointToPut(board, i, j) == false) {
        continue;
      }

      virtual_board = board;
      virtual_board.set_stone(i, j, own_stone());
      if (alpha_beta == true) {
        current_min_max = AlphaBeta(virtual_board, count, max_min_max);
      } else {
        current_min_max = MiniMax(virtual_board, count);
      }
      if (current_min_max > max_min_max) {
        max_min_max = current_min_max;
        put_point = Position(i, j);
      }
    }
  }
  return put_point;
}

int Brain::MiniMax(Board board, int depth) {
  int max, min, score;
  
  if (depth == 0 || board.StoneNum() == kBoardSize * kBoardSize) {
    return Heuristic(board);
  }

  if (board.StoneToPut() == own_stone()) {
    max = INT_MIN;
    for (int i = 0; i < kBoardSize; i++) {
      for (int j = 0; j < kBoardSize; j++) {
        if (IsPointToPut(board, i, j) == false) {
          continue;
        }
        
        score = MiniMax(board.CopyWithPoint(i, j), depth - 1);
        if (score > max) {
          max = score;
        }
      }
    }
    return max;
  } else {
    min = INT_MAX;
    for (int i = 0; i < kBoardSize; i++) {
      for (int j = 0; j < kBoardSize; j++) {
        if (IsPointToPut(board, i, j) == false) {
          continue;
        }
        
        score = MiniMax(board.CopyWithPoint(i, j), depth - 1);
        if (score < min) {
          min = score;
        }
      }
    }
    return min;
  }
}

int Brain::AlphaBeta(Board board, int depth, int cut) {
  if (depth == 0 || board.StoneNum() == kBoardSize * kBoardSize) {
    return Heuristic(board);
  }
  return 0;
}

Position Brain::GetEmptyPoint(Board board) {
  int center = kBoardSize;
  if (center % 2 != 0) {
    center--;
  }
  center /= 2;

  for (int limit = 0; limit < center; limit++) {
    for (int i = center - limit; i <= center + limit; i++) {
      for (int j = center - limit; j <= center + limit; j++) {
        if (board.stone(i, j) == kStoneBlank) {
          return Position(i, j);
        }
      }
    }
  }
  return Position::Null();
}

StoneType Brain::OppositeStone(StoneType stone) {
  if (stone == kStoneBlack) {
    return kStoneWhite;
  } else {
    return kStoneBlack;
  }
}
