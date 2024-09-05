/*#include "graphicsdisplay.h"

GraphicsDisplay::GraphicsDisplay (int n, int size): n{n}, size{size} {
    w.fillRectangle(0, 0, size, size, Xwindow::White);
}


void GraphicsDisplay::notify(std::vector<std::vector<std::shared_ptr<Piece> > > &theboard) {

 for (int i = 0; i < n; i++ ){
        for (int j = 0; j < n ; j++){
            // check if the square if empty or not ' '
            if ( (i + j) % 2 == 0){
		w.fillRectangle(j * size / n, (7-i)* size / n, size/n, size / n, Xwindow::Green);
                } else {
                    // ' '
                    w.fillRectangle(j * size / n, (7-i) * size / n, size / n,size/n,  Xwindow::White);
                }

	     if(theboard.at(i).at(j) != nullptr) {
                int temp = 7-i;
                if (theboard.at(i).at(j).get()->getColour()){ // Black pieces
                    if (theboard.at(i).at(j).get()->getType() == 'K'){
                        w.drawString( j * size / n + (size/n)/2, temp* size / n+ (size/n)/2, "k");
                    } else if (theboard.at(i).at(j).get()->getType() == 'Q'){
                        w.drawString( j * size / n + (size/n)/2, temp* size / n+ (size/n)/2, "q");
                    } else if (theboard.at(i).at(j).get()->getType() == 'B'){
                        w.drawString( j * size / n + (size/n)/2, temp* size / n+ (size/n)/2, "b");
                    } else if (theboard.at(i).at(j).get()->getType() == 'R'){
                        w.drawString( j * size / n + (size/n)/2, temp* size / n+ (size/n)/2, "r");
                    } else if (theboard.at(i).at(j).get()->getType() == 'N') {
                        w.drawString( j * size / n + (size/n)/2, temp* size / n+ (size/n)/2, "n");
                    } else if (theboard.at(i).at(j).get()->getType() == 'P') {
                        w.drawString( j * size / n + (size/n)/2, temp* size / n+ (size/n)/2, "p");
                    }
			    } else { // White piece
			        if (theboard.at(i).at(j).get()->getType() == 'K'){
                        w.drawString( j * size / n + (size/n)/2, temp* size / n+ (size/n)/2, "K");

                    } else if (theboard.at(i).at(j).get()->getType() == 'Q'){
                        w.drawString( j * size / n + (size/n)/2, temp* size / n+ (size/n)/2, "Q");
                    } else if (theboard.at(i).at(j).get()->getType() == 'B'){
                        w.drawString( j * size / n + (size/n)/2, temp* size / n+ (size/n)/2, "B");
                    } else if (theboard.at(i).at(j).get()->getType() == 'R'){
                        w.drawString( j * size / n + (size/n)/2, temp* size / n+ (size/n)/2, "R");
                    } else if (theboard.at(i).at(j).get()->getType() == 'N') {
                        w.drawString( j * size / n + (size/n)/2, temp* size / n+ (size/n)/2, "N");
                    } else if (theboard.at(i).at(j).get()->getType() == 'P') {
                        w.drawString( j * size / n + (size/n)/2, temp * size / n+ (size/n)/2, "P");
                    }
                }
         }
        }
 }
}
*/