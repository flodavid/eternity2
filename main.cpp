#include <QApplication>

#include "mainwindow.h"
#include "Model/piece_model.h"
#include "Model/instance_model.h"
#include "Model/board_model.h"
#include "View/board_view.h"

using namespace std;

int main(int argc, char *argv[])
{
    /**
     * Utiliser le bloc ci-dessous pour Appli Qt
     * @return
     */
//    /*
    QApplication a(argc, argv);



/// TODO A placer dans mainwindow
    Configuration* config= new Configuration;
    config->tryLoadFile("../eternity2/instances_puzzles/pieces_04x04.txt");
    cout << "Instance (configuration) statique" << endl;
//    cout << ((Instance)config)<< endl;

    Board* board_model= new Board(config);
    MainWindow w(board_model);
    w.show();

    w.showBoard();

    return a.exec();
//    */

    /**
      * Utiliser le bloc suivant pour debug console
      * @return
      */

    // Test piece et instance

    int tab_tmp[4];

    for(int i = 0; i < 4; ++i ){
        tab_tmp[i] =  (1+i)*5;
    }

    Piece *p = new Piece(1, tab_tmp);
    cout << *p;

    cout << "Fin test piece, statique et pointeur ok" << endl;

    Instance *inst2 = new Instance();
    inst2->tryLoadFile("../eternity2/instances_puzzles/pieces_03x03.txt");
    cout << "Pointeur sur instance" << endl;
    cout << *inst2 ;
    cout << "Fin test instance, statique et pointeur ok" << endl;

    return 0;

}
