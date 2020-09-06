#include <QAction>
#include <QDebug>
#include <math.h>
#include <QPainter>
#include <QTimer>
#include <QSound>
#include <QMouseEvent>
#include <QMessageBox>
#include <QMenuBar>
#include <QMenu>
#include "mainwindow.h"

const int EdgeWidth = 30;
const int ChessR = 15;
const int MarkR = 6;
const int BlockR = 40;
const int M_TolerateR = 20;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    setFixedSize(EdgeWidth * 2 + BlockR * kBoardSizeNum, EdgeWidth * 2 + BlockR * kBoardSizeNum);
    setMouseTracking(true);

    QMenu *gameMenu = menuBar()->addMenu(tr("Game"));


    QAction *actionPVE = new QAction("New Game", this);
    connect(actionPVE, SIGNAL(triggered()), this, SLOT(initPVEGame()));
    gameMenu->addAction(actionPVE);

    initGame();
}

MainWindow::~MainWindow()
{
    if (game)
    {
        delete game;
        game = nullptr;
    }
}

void MainWindow::initGame()
{   
    // 初始化游戏模型
    game = new GameModel;
    initPVPGame();
}

void MainWindow::initPVPGame()
{
    game_type = PERSON;
    game->gameStatus = PLAYING;
    game->startGame(game_type);
    update();
}

void MainWindow::initPVEGame()
{
    game_type = BOT;
    game->gameStatus = PLAYING;
    game->startGame(game_type);
    update();
}

void MainWindow::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, true);
    for (int i = 0; i < kBoardSizeNum + 1; i++)
    {
        painter.drawLine(EdgeWidth + BlockR * i, EdgeWidth, EdgeWidth + BlockR * i, size().height() - EdgeWidth);
        painter.drawLine(EdgeWidth, EdgeWidth + BlockR * i, size().width() - EdgeWidth, EdgeWidth + BlockR * i);
    }

    QBrush brush;
    brush.setStyle(Qt::SolidPattern);
    if (clickPosRow > 0 && clickPosRow < kBoardSizeNum &&
        clickPosCol > 0 && clickPosCol < kBoardSizeNum &&
        game->gameMapVec[clickPosRow][clickPosCol] == 0)
    {
        if (game->playerFlag)
            brush.setColor(Qt::green);
        else
            brush.setColor(Qt::green);
        painter.setBrush(brush);
        painter.drawRect(EdgeWidth + BlockR * clickPosCol - MarkR / 2, EdgeWidth + BlockR * clickPosRow - MarkR / 2, MarkR, MarkR);
    }

    // 绘制棋子 
    for (int i = 0; i < kBoardSizeNum; i++)
        for (int j = 0; j < kBoardSizeNum; j++)
        {
            if (game->gameMapVec[i][j] == 1)
            {
                brush.setColor(Qt::white);
                painter.setBrush(brush);
                painter.drawEllipse(EdgeWidth + BlockR * j - ChessR, EdgeWidth + BlockR * i - ChessR, ChessR * 2, ChessR * 2);
            }
            else if (game->gameMapVec[i][j] == -1)
            {
                brush.setColor(Qt::black);
                painter.setBrush(brush);
                painter.drawEllipse(EdgeWidth + BlockR * j - ChessR, EdgeWidth + BlockR * i - ChessR, ChessR * 2, ChessR * 2);
            }
        }

    // 判断输赢
    if (clickPosRow > 0 && clickPosRow < kBoardSizeNum &&
        clickPosCol > 0 && clickPosCol < kBoardSizeNum &&
        (game->gameMapVec[clickPosRow][clickPosCol] == 1 ||
            game->gameMapVec[clickPosRow][clickPosCol] == -1))
    {
        if (game->isWin(clickPosRow, clickPosCol) && game->gameStatus == PLAYING)
        {
            qDebug() << "win";
            game->gameStatus = WIN;

            QString str;
            if (game->gameMapVec[clickPosRow][clickPosCol] == 1)
                str = "玩家";
            else if (game->gameMapVec[clickPosRow][clickPosCol] == -1)
                str = "AI";
            QMessageBox::StandardButton btnValue = QMessageBox::information(this, "恭喜", str + " 赢了!");
            if (btnValue == QMessageBox::Ok)
            {
                game->startGame(game_type);
                game->gameStatus = PLAYING;
            }
        }
    }
    if (game->isDeadGame())
    {

        QMessageBox::StandardButton btnValue = QMessageBox::information(this, "oops", "dead game!");
        if (btnValue == QMessageBox::Ok)
        {
            game->startGame(game_type);
            game->gameStatus = PLAYING;
        }

    }
}

void MainWindow::mouseMoveEvent(QMouseEvent *event)
{   
    int x = event->x();
    int y = event->y();

    if (x >= EdgeWidth + BlockR / 2 &&
            x < size().width() - EdgeWidth &&
            y >= EdgeWidth + BlockR / 2 &&
            y < size().height()- EdgeWidth)
    {
        int col = x / BlockR;
        int row = y / BlockR;

        int leftTopPosX = EdgeWidth + BlockR * col;
        int leftTopPosY = EdgeWidth + BlockR * row;


        clickPosRow = -1;
        clickPosCol = -1;
        int len = 0;

        len = sqrt((x - leftTopPosX) * (x - leftTopPosX) + (y - leftTopPosY) * (y - leftTopPosY));
        if (len < M_TolerateR)
        {
            clickPosRow = row;
            clickPosCol = col;
        }
        len = sqrt((x - leftTopPosX - BlockR) * (x - leftTopPosX - BlockR) + (y - leftTopPosY) * (y - leftTopPosY));
        if (len < M_TolerateR)
        {
            clickPosRow = row;
            clickPosCol = col + 1;
        }
        len = sqrt((x - leftTopPosX) * (x - leftTopPosX) + (y - leftTopPosY - BlockR) * (y - leftTopPosY - BlockR));
        if (len < M_TolerateR)
        {
            clickPosRow = row + 1;
            clickPosCol = col;
        }
        len = sqrt((x - leftTopPosX - BlockR) * (x - leftTopPosX - BlockR) + (y - leftTopPosY - BlockR) * (y - leftTopPosY - BlockR));
        if (len < M_TolerateR)
        {
            clickPosRow = row + 1;
            clickPosCol = col + 1;
        }
    }

    update();
}

void MainWindow::mouseReleaseEvent(QMouseEvent *event)
{
    if (!(game_type == BOT && !game->playerFlag))
    {
        chessOneByPerson();
        if (game->gameType == BOT && !game->playerFlag)
            chessOneByAI();

    }

}

void MainWindow::chessOneByPerson()
{
    if (clickPosRow != -1 && clickPosCol != -1 && game->gameMapVec[clickPosRow][clickPosCol] == 0)
    {
        game->actionByPerson(clickPosRow, clickPosCol);
        update();
    }
}

void MainWindow::chessOneByAI()
{
    game->actionByAI(clickPosRow, clickPosCol);
    update();
}

