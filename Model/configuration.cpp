#include "configuration.h"

using namespace std;

Configuration::Configuration(const Instance* _instance) :
    instance(_instance)
{}

Configuration::Configuration(const string& fileName)
{
    Instance* inst= new Instance();

    if (!inst->tryLoadFile(fileName)){
        cerr << "Impossible de charger le fichier " << fileName << endl;
        exit(EXIT_FAILURE);
    }
    else { instance = inst; }
}

Configuration::Configuration(const Configuration &C) {
    instance = C.instance;
    positions = C.positions;
}

const pair<int, int>& Configuration::getPair(int x, int y) const {
    // Cas où on essaye d'atteindre une pièce à l'extérieur du bord du plateau
    if(x == -1 || x == get_width() || y == -1 || y == get_height()) {
        const pair<int, int>& position = make_pair(0, -1);
        return position;
    }
    // Cas où on essaye d'atteindre une cellule en dehors du plateau qui n'est pas un bord
    else if(x < -1 || x > get_width() || y < -1 || y > get_height()) {
        cerr << "ERROR getPair : Traitement d'une case en dehors du plateau (" << x << "," << y << ") (renvoie d'une pair(0,-1)) " << endl;
        throw out_of_range("getPair");
    }
    const pair<int, int>& position = positions[x + y * instance->get_width()];
    return position;
}

const Piece & Configuration::getPiece(int x, int y) const {
    try{
        const Piece & piece = getPiece(getPair(x, y).id);
        return piece;
    } catch (out_of_range oor) {
        throw out_of_range("getPiece");
    }
}

const Piece & Configuration::getPiece(int id) const {
    const Piece & piece = instance->getPiece(id);
    return piece;
}

PairColors * Configuration::getRotatedMotif(int x, int y) const {
    if(x < 0 || x >= get_width() || y < 0 || y >= get_height()) {
        cerr << "ERROR getRotatedMotif : Traitement d'une case en dehors du plateau"<< endl;
        throw out_of_range("getPiece");
    }
    pair<int, int> piece_pair = getPair(x, y);
    Piece piece = getPiece(x, y);
    return piece.rotate(piece_pair.rot);
}

PairColors * Configuration::getRotatedMotif(int pos) const
{
    if(pos >= (signed) positions.size()) {
        cerr << "ERROR getRotatedMotif : Traitement d'une case en dehors du plateau"<< endl;
        throw out_of_range("getPiece");
    }
    pair<int, int> piece_pair = positions[pos];
    Piece piece = getPiece(piece_pair.id);
    return piece.rotate(piece_pair.rot);
}

int Configuration::getPosition(const Piece& p) const {
    bool found = false;
    int ind = 0;
    while(!found) {
        if(positions[ind].first == p.get_id())
            found = true;
        else ind++;
    }
    return ind;
}

int Configuration::getPosition(const int id) const {
    int ind = -1;
    for(unsigned int i=0 ; i<positions.size() ; ++i) {
        if(positions[i].first == id)
            ind = (int)i;
    }
    return ind;
}

pair<int, int> Configuration::getCase(int id) const {
    int pos = getPosition(id);
    int processed_case = 0;
    for(int j=0 ; j<get_height() ; j++) {
        for(int i=0 ; i<get_width() ; i++) {
            if(processed_case == pos) return make_pair(i, j);
            processed_case++;
        }
    }
    return make_pair(-1, -1);
}

vector<pair<int, int>> Configuration::getAdjacent(int x, int y) const {
    vector<pair<int, int> > swne(4);

    pair<int, int> pos;
    // Récupération Sud
    try {
        pos = getPair(x, y+1);
    } catch ( const std::exception & e ) {
        cerr << "Récupération Ouest" << endl;
        pos = make_pair(-1,-1);
    }
    swne[0] = pos;
    // Récupération Est
    try {
        pos = getPair(x-1, y);
    } catch ( const std::exception & e ) {
        cerr << "Récupération Est" << endl;
        pos = make_pair(-1,-1);
    }
    swne[1] = pos;
    // Récupération Nord
    try {
        pos = getPair(x, y-1);
    } catch ( const std::exception & e ) {
        cerr << "Récupération Nord" << endl;
        pos = make_pair(-1,-1);
    }
    swne[North] = pos;
    // Récupération Ouest
    try {
        pos = getPair(x+1, y);
    } catch ( const std::exception & e ) {
        cerr << "Récupération Ouest" << endl;
        pos = make_pair(-1,-1);
    }
    swne[3] = pos;

    return swne;
}

ostream& Configuration::print(ostream& out) const{
    for(int i=0 ; i<instance->get_width() ; ++i) {
        for(int j=0 ; j<instance->get_height() ; ++j) {
            pair<int, int> p = getPair(i, j);

            out << "Case (" << i << "," << j << ") : \tID = " << p.first << "\tRotation = " << p.second << "\tSONE = ";

            const Piece& piece = getPiece(i, j);

            const PairColors* swne ;
            if(p.second != 0) {
                swne = piece.rotate(p.second );
            } else {
                swne = piece.get_motif();
            }

            for(int l=0; l<MAX_CARD; ++l){
                if(l==0)
                    out << "[" << swne[l] << ",";
                else if(l==MAX_CARD-1)
                    out << swne[l] << "]";
                else
                    out << swne[l] << ",";
            }
            out << endl;
        }
    }

    for(int i=0 ; i<instance->get_width(); i++) {
        for(int j=0 ; j<instance->get_height() ; j++) {
            pair<int, int> p = getPair(j, i);
            cout << "\t" << p.first << " ";
        }
        cout << endl;
    }

    return out;
}

void Configuration::setPiece(int x, int y, pair<int, int> pos) {
    if(pos.second < 0) {
        setPiece(x, y, make_pair(pos.first, pos.second+4));
        return;
    }
    if(pos.second > 3)
        pos.second = pos.second % 4;
    positions[x + y*get_width()] = pos;
}

void Configuration::rotatePiece(int x, int y, int degree) {
    if(degree < 0)                  // Si la rotation demandée est inférieure à 0
        rotatePiece(x, y, degree+4);
    else if(degree >= 4) // Sinon si la rotation demandée est supérieure à 4
        rotatePiece(x, y, degree % 4);
    else {
        positions[x + y * get_width()].second = positions[x + y * get_width()].second + degree;
    }
}

bool Configuration::tryLoadFile(const string &fileName){
    if( instance->get_width() * instance->get_height() == 0){
        cerr << "Aucune instance n'est chargée" << endl;
        return false;
    }else{
        ifstream f(fileName.c_str());

        if(!f){
            cerr << "Erreur pendant l'ouverture du fichier" << endl;
            return false;
        } else {

            string line;

            while(getline(f,line)){

                vector<string>& tokens = explode(line);

                positions.push_back( pair<int,int>( atoi(tokens[0].c_str()) , atoi(tokens[1].c_str()) ) );
            }

            if(positions.size() != (unsigned)(instance->get_width() * instance->get_height()) ){
                cerr << "Fichier de configuration incomplet" << endl;
                return false;
            } else {
                return true;
            }
        }
    }
}

void Configuration::randomConfiguration() {
    int i_rot;
    int p_id;
    vector<int> border_pieces;
    vector<int> edge_pieces;
    vector<int> pieces_remaining;

    // Travail uniquement sur l'id des pieces
    for(Piece p : (*getPieces()) ) {
        if(p.isBorder())
            border_pieces.push_back(p.get_id());
        else if(p.isEdge())
            edge_pieces.push_back(p.get_id());
        else
            pieces_remaining.push_back(p.get_id());
    }

    // Mélange aléatoire
    random_shuffle(border_pieces.begin(), border_pieces.end());
    random_shuffle(edge_pieces.begin(), edge_pieces.end());
    random_shuffle(pieces_remaining.begin(), pieces_remaining.end());

    for(int j=0 ; j<get_height() ; j++) {
        for(int i=0 ; i<get_width() ; i++) {
            i_rot = 0;
            if((i == 0 && j ==0)||(i==0 && j==get_height()-1)||(i==get_width()-1 && j==0)||(i==get_width()-1 && j==get_height()-1)) {
                // Angles
                p_id = edge_pieces.back();
                edge_pieces.pop_back();
                placePiece(make_pair(p_id, i_rot));
                // Bonne rotation
                while(!isConstraintEdgesRespected(i, j))
                    setPiece(i, j, make_pair(p_id, ++i_rot));
            } else if(i == 0 || j == 0 || i == get_width()-1 || j == get_height()-1){
                // Border
                p_id = border_pieces.back();
                border_pieces.pop_back();
                placePiece(make_pair(p_id, i_rot));
                // Bonne rotation
                while(!isConstraintRowsXtremRespected(i,j) || !isConstraintColsXtremRespected(i,j)) {
                    setPiece(i, j, make_pair(p_id, ++i_rot));
                }
            } else {
                // Autre
                p_id = pieces_remaining.back();
                pieces_remaining.pop_back();
                // Rotation aléatoire
                i_rot = rand() % 4;
                // Ajout de la pair
                placePiece(make_pair(p_id, i_rot));
            }
        }
    }
}

vector<Configuration*>&  Configuration::generateRandomConfigurations(const Instance * instance, int limit) {
    vector<Configuration*>& configurations= *(new vector<Configuration*>);

    for(int ind_conf=0 ; ind_conf < limit ; ind_conf++) {
        Configuration* configuration= new Configuration(instance);
        configuration->randomConfiguration();

        configurations.push_back(configuration);
    }

    return configurations;
}

PairColors* Configuration::get_rotated_motifs(int current_piece_indice) const{
    const Piece& current_piece= instance->getPiece(current_piece_indice);
    int rotation = positions[getPosition(current_piece)].second;

    return current_piece.rotate(rotation);
}

PairColors &Configuration::getNorthMotifSouthPiece(int current_piece_indice) const {
    // Récupère le motif Nord de la pièce au sud de la piece courante tournée
    // piece sud de piece courante = position piece courante + width
    return get_rotated_motifs(current_piece_indice + get_width())[North];
}

PairColors &Configuration::getWestMotifEastPiece(int current_piece_indice) const {
    // Récupère le motif Ouest de la pièce à l'Est de la piece courante tournée
    // piece est de piece courante = position piece courante + 1
    return get_rotated_motifs(current_piece_indice + 1)[West];
}

const Piece& Configuration::getClosePiece(int current_piece, Cardinal neightboor_card) const {
    switch(neightboor_card){
    case South:
    {
        const Piece& south_piece= instance->getPiece(current_piece + get_width());
        return south_piece;
    }
        break;
    case West:
    {
        const Piece& west_piece= instance->getPiece(current_piece - 1);
        return west_piece;
    }
        break;
    case North:
    {
        const Piece& north_piece= instance->getPiece(current_piece - get_width());
        return north_piece;
    }
        break;
    case East:
    {
        const Piece& east_piece= instance->getPiece(current_piece + 1);
        return east_piece;
    }
        break;
    default:
        break;
    }

//retourne la meme piece si probleme avec le switch
const Piece& error_piece= instance->getPiece(current_piece);
return error_piece;
}

PairColors Configuration::getMotifClosePiece(int current_piece, Cardinal neightboor_card, Cardinal motif_card) const {
    const Piece& neightboor_piece= getClosePiece(current_piece, neightboor_card);
    //piece
    return neightboor_piece.get_motif()[motif_card];
}

/** Vérifiée **/
bool Configuration::motifs_match(PairColors first_motif, PairColors second_motif) const
{
#if DEBUG_MOTIF_MATCH
    cout<< "Comparaison : " << first_motif << " - " << second_motif << endl;
#endif
    if (first_motif != second_motif || first_motif == Black_Black){
        #if DEBUG_MOTIF_MATCH
        cout << "Une erreur.";
        #endif
        return false;
    }
    else {
        #if DEBUG_MOTIF_MATCH
        cout << "Pas d'erreur. ";
        #endif
        return true;
    }

//    if(first_motif == Black_Black) return false;
//    if(second_motif == Black_Black) return false;
//    if(first_motif != second_motif) return false;
//    return true;
}

int Configuration::countNbErrors() const{
    int nb_errors= 0;

    int begin_last_raw= get_height()*get_width() -get_width(); // première case de la derniere ligne

    // Parcours de toutes les cases du plateau, dernière case exceptée
    for(int i=0; i < (get_height()*get_width()-1); ++i){
        int current_x= i % get_width();
//        int current_y= i / get_width();

        PairColors current_piece_east_motif= getRotatedMotif(i)[East];
        PairColors current_piece_south_motif= getRotatedMotif(i)[South];

        #if DEBUG_CHECK_PIECES
        cout << "Piece n°" << i << endl;
        #endif

        //derniere colonne
        if(current_x == get_width()-1){
            PairColors south_piece_north_motif= getRotatedMotif(i+get_width())[North];
            if(!motifs_match(current_piece_south_motif, south_piece_north_motif)){
                #if DEBUG_CHECK_PIECES
                cout << "Erreur piece COURANTE - piece SUD" << endl;
                #endif
                ++nb_errors;
            }
        }
        //derniere ligne
        else if(i >= begin_last_raw){
            PairColors east_piece_west_motif= getRotatedMotif(i+1)[West];
            if(!motifs_match(current_piece_east_motif, east_piece_west_motif)){
                #if DEBUG_CHECK_PIECES
                cout << "Erreur piece COURANTE - piece EST" << endl;
                #endif
                ++nb_errors;
            }
        }
        //ni derniere ligne ni derniere colonne
        else {
            PairColors south_piece_north_motif= getRotatedMotif(i+get_width())[North];
            if(!motifs_match(current_piece_south_motif, south_piece_north_motif)){
                #if DEBUG_CHECK_PIECES
                cout << "Erreur piece COURANTE - piece SUD" << endl;
                #endif
                ++nb_errors;
            }

            PairColors east_piece_west_motif= getRotatedMotif(i+1)[West];
            if(!motifs_match(current_piece_east_motif, east_piece_west_motif)){
                #if DEBUG_CHECK_PIECES
                cout << "Erreur piece COURANTE - piece EST" << endl;
                #endif
                ++nb_errors;
            }
        }
    }
    return nb_errors;
}


bool Configuration::pieces_match(int indice_current_piece, Cardinal direction_neightboor_piece){
    Piece current_piece= getPiece(positions[indice_current_piece].first);
    const PairColors* motifs_current_piece= current_piece.get_motif();
    PairColors motif_current_piece= motifs_current_piece[direction_neightboor_piece];

    //comparaison piece courante / piece voisine
    switch(direction_neightboor_piece){
    case South:
    {
        PairColors north_motif_neightboor_piece= getMotifClosePiece(indice_current_piece, South, North);
        if(motifs_match(motif_current_piece, north_motif_neightboor_piece)) return true;
        else return false;
    }
        break;
    case West:
    {
        PairColors east_motif_neightboor_piece= getMotifClosePiece(indice_current_piece, West, East);
        if(motifs_match(motif_current_piece, east_motif_neightboor_piece)) return true;
        else return false;
    }
        break;
    case North:
    {
        PairColors south_motif_neightboor_piece= getMotifClosePiece(indice_current_piece, North, South);
        if(motifs_match(motif_current_piece, south_motif_neightboor_piece)) return true;
        else return false;
    }
        break;
    case East:
    {
        PairColors west_motif_neightboor_piece= getMotifClosePiece(indice_current_piece, East, West);
        if(motifs_match(motif_current_piece, west_motif_neightboor_piece)) return true;
        else return false;
    }
        break;
    default:
        break;
    }
return false;
}

int Configuration::getPieceNbErrors(const Piece& current_piece) {
    int nb_errors= 0;
    int indice_current_piece= current_piece.get_id();

    //position de la piece courante (combien de voisins ?)
    int current_piece_pos= getPosition(indice_current_piece);
    int current_x= current_piece_pos % get_width();
    int current_y= current_piece_pos / get_width();

    if(0 == current_x) //premiere ligne
    {
        if(0 == current_y) //coin nord gauche
        {
            //comparaison piece courante / piece est
            if(!pieces_match(indice_current_piece, East)) ++nb_errors;

            //comparaison piece courante / piece sud
            if(!pieces_match(indice_current_piece, South)) ++nb_errors;

        } else if(get_width()-1 == current_y) //coin nord droit
        {
            //comparaison piece courante / piece ouest
            if(!pieces_match(indice_current_piece, West)) ++nb_errors;

            //comparaison piece courante / piece sud
            if(!pieces_match(indice_current_piece, South)) ++nb_errors;

        } else //bord nord
        {
            //comparaison piece courante / piece ouest
            if(!pieces_match(indice_current_piece, West)) ++nb_errors;

            //comparaison piece courante / piece sud
            if(!pieces_match(indice_current_piece, South)) ++nb_errors;

            //comparaison piece courante / piece est
            if(!pieces_match(indice_current_piece, East)) ++nb_errors;

        }
    } else if(get_height()-1 == current_x) // derniere ligne
    {
        if(0 == current_y) //coin sud gauche
        {
            //comparaison piece_courante / piece nord
            if(!pieces_match(indice_current_piece, North)) ++nb_errors;

            //comparaison piece courante / piece est
            if(!pieces_match(indice_current_piece, East)) ++nb_errors;

        } else if(get_width()-1 == current_y) //coin sud droit
        {
            //comparaison piece courante / piece ouest
            if(!pieces_match(indice_current_piece, West)) ++nb_errors;

            //comparaison piece courante / piece nord
            if(!pieces_match(indice_current_piece, North)) ++nb_errors;

        } else //bord sud
        {
            //comparaison piece courante / piece ouest
            if(!pieces_match(indice_current_piece, West)) ++nb_errors;

            //comparaison piece courante / piece nord
            if(!pieces_match(indice_current_piece, North)) ++nb_errors;

            //comparaison piece courante / piece est
            if(!pieces_match(indice_current_piece, East)) ++nb_errors;

        }
    } else //ni premiere colonne ni premiere ligne
    {
        if(0 == current_y) // bord ouest
        {
            //comparaison piece courante / piece nord
            if(!pieces_match(indice_current_piece, North)) ++nb_errors;

            //comparaison piece courante / piece est
            if(!pieces_match(indice_current_piece, East)) ++nb_errors;

            //comparaison piece courante / piece sud
            if(!pieces_match(indice_current_piece, South)) ++nb_errors;

        } else if(get_height()-1 == current_y) // bord est
        {
            //comparaison piece courante / piece nord
            if(!pieces_match(indice_current_piece, North)) ++nb_errors;

            //comparaison piece courante / piece ouest
            if(!pieces_match(indice_current_piece, West)) ++nb_errors;

            //comparaison piece courante / piece sud
            if(!pieces_match(indice_current_piece, South)) ++nb_errors;

        } else // interieur puzzle (pieces avec 4 voisins)
        {
            //comparaison piece courante / piece nord
            if(!pieces_match(indice_current_piece, North)) ++nb_errors;

            //comparaison piece courante / piece ouest
            if(!pieces_match(indice_current_piece, West)) ++nb_errors;

            //comparaison piece courante / piece sud
            if(!pieces_match(indice_current_piece, South)) ++nb_errors;

            //comparaison piece courante / piece est
            if(!pieces_match(indice_current_piece, East)) ++nb_errors;
        }
    }

    return nb_errors;
}

int Configuration::isBestPlaced(int current_piece_id){
    std::pair<int, int> coord_current_piece= getCase(current_piece_id);
    int current_piece_x= coord_current_piece.first;
    int current_piece_y= coord_current_piece.second;

    //tester si en tournant la piece indice_piece d'une rotation de val_rot, on obtient moins d'erreurs avec getPieceNbErrors
    const Piece& current_piece= getPiece(current_piece_x, current_piece_y);
    Piece piece_test= current_piece;
    int local_nb_errors= getPieceNbErrors(piece_test);

    //paire d'entiers : <rotation appliquee, nombre d'erreurs liees a la rotation appliquee>
    std::pair<int, int> best_rotation= std::make_pair(0, local_nb_errors);
    for(int i= 1; i < 4; ++i){
        piece_test.rotation(1);
        if(best_rotation.second > getPieceNbErrors(piece_test)){
            best_rotation.first= i;
            best_rotation.second= getPieceNbErrors(piece_test);
        }
    }
return best_rotation.first;
}
