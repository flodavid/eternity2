#include "piece.h"

using namespace std;

Piece::Piece(int _id, PairColors tab[]) :
    id(_id)
{
    for(int i=0; i<MAX_CARD; ++i){
        motif[i] = tab[i];
    }
}

bool Piece::isEdge() {
    int count_black = 0;
    for(int i=0 ; i<MAX_CARD ; ++i)
        if(motif[i] == 0) count_black++;
    return count_black==2;
}

bool Piece::isBorder() {
    int count_black = 0;
    for(int i=0 ; i<MAX_CARD ; ++i)
        if(motif[i] == 0) count_black++;
    return count_black == 1;
}

ostream& Piece::print(ostream& out) const{
    out << "Pièce n° " << id << " : ";

    // Affichage des n-1 premières couleurs chacune suivie d'un espace
    for(int i = 0; i < MAX_CARD-1; ++i)
        out << motif[i] << " ";

    // Affichage de la n-ième couleur suivie d'un ; et d'un saut de ligne
    out << motif[MAX_CARD-1] << ";" << endl;

    return out;
}

PairColors *Piece::rotate(int nbRotation) const{
    PairColors* rotated_motifs = new PairColors;
    copy(motif, motif+4, rotated_motifs);

    for(int i=0 ; i < nbRotation ; i++)
        std::rotate(rotated_motifs, rotated_motifs+3 , rotated_motifs+4);

    return rotated_motifs;
}

void Piece::rotation(int val_rotation){
    switch(val_rotation){
    case 1:
        set_motifs(motif[East], motif[South], motif[West], motif[North]);
        break;
    case 2:
        set_motifs(motif[North], motif[East], motif[South], motif[West]);
        break;
    case 3:
        set_motifs(motif[West], motif[North], motif[East], motif[South]);
        break;
    default:
        break;
    }
}
