#ifndef Person_Relations_positions_h
#define Person_Relations_positions_h
float numinator = TOTAL_FRAMES * TOTAL_FRAMES / 4;
typedef struct{
    float x;
    float y;
    float z;
} coordinates;

typedef struct{
    int x;
    int y;
} FlatCoords;

int count = 1;
bool overHalf;
bool animate;

double drand (void) {
    double d;
    do {
        d = (((rand () * RS_SCALE) + rand ()) * RS_SCALE + rand ()) * RS_SCALE;
    } while (d >= 1); /* Round off */
    return d;
}

#define irand(x) ((unsigned int) ((x) * drand ())) //Returns a integer between [0,r)

void create_coords(coordinates *fill, int size){
    // First, create coords
    for(int i = 0; i < size; i++){
		if (rand() % 2 == 0){
			fill[i].x = -(float)irand(100);
		}
		else
		{
			fill[i].x = (float)irand(100);
		}
		if (rand() % 2 == 0){
			fill[i].y = -(float)irand(100);
		}
		else
		{
			fill[i].y = (float)irand(100);
		}
        switch (irand(4)) {
            case 1:
                fill[i].z = (float)0;
                break;
            case 2:
                fill[i].z = -(float)30;
                break;
            case 3:
                fill[i].z = -(float)60;
                break;
            case 4:
                fill[i].z = -(float)80;
                break;
            }
    }
}
void CheckNodeVisible(coordinates *check, int size, bool *write, coordinates campos){
    for (int i = 0; i < size; i++){
		if(check[i].z == 0){
            if(check[i].x >= ((0.84375 * campos.z) - campos.x) && check[i].x <= (-(0.84375 * campos.z) - campos.x)){
                if(check[i].y >= ((0.45 * campos.z) - campos.y) && check[i].y <= (-(0.45 * campos.z) - campos.y)){
                    write[i] = true;
                }
                else(write[i] = false);
            }
            else(write[i] = false);
        }
        if(check[i].z == -30){
            if(check[i].x >= ((0.84375 * campos.z) - campos.x - 15) && check[i].x <= (-(0.84375 * campos.z) - campos.x + 15)){
                if(check[i].y >= ((0.45 * campos.z) - campos.y - 15) && check[i].y <= (-(0.45 * campos.z) - campos.y + 15)){
                    write[i] = true;
                }
                else(write[i] = false);
            }
            else(write[i] = false);
        }if(check[i].z == -60){
            if(check[i].x >= ((0.84375 * campos.z) - campos.x - 30) && check[i].x <= (-(0.84375 * campos.z) - campos.x + 30)){
                if(check[i].y >= ((0.45 * campos.z) - campos.y - 30) && check[i].y <= (-(0.45 * campos.z) - campos.y + 30)){
                    write[i] = true;
                }
                else(write[i] = false);
            }
            else(write[i] = false);
        }if(check[i].z == -80){
            if(check[i].x >= ((0.84375 * campos.z) - campos.x - 40) && check[i].x <= (-(0.84375 * campos.z) - campos.x + 40)){
                if(check[i].y >= ((0.45 * campos.z) - campos.y - 40) && check[i].y <= (-(0.45 * campos.z) - campos.y) + 40){
                    write[i] = true;
                }
                else(write[i] = false);
            }
            else(write[i] = false);
        }
	}
}

void subtle_coords_change(coordinates *base, coordinates *final, int size){
    for(int i = 0; i < size; i++){
        if (rand() % 2 == 0){
			final[i].x = -(float)irand(5) + base[i].x;
		}
		else
		{
            final[i].x = (float)irand(5) + base[i].x;
		}
		if (rand() % 2 == 0){
			final[i].y = -(float)irand(5) + base[i].y;
		}
		else
		{
			final[i].y = (float)irand(5) + base[i].y;
		}
        final[i].z = base[i].z;
    }
}
void edit_coords_highlight(coordinates *fill, int size, int highlighta, int highlightb){
	for (int i = 0; i < size; i++){
		if (rand() % 2 == 0){
			fill[i].x = -(float)irand(100);
		}
		else
		{
			fill[i].x = (float)irand(100);
		}
		if (rand() % 2 == 0){
			fill[i].y = -(float)irand(100);
		}
		else
		{
			fill[i].y = (float)irand(100);
		}
		switch (irand(4)) {
		case 1:
			fill[i].z = (float)0;
			break;
		case 2:
			fill[i].z = -(float)30;
			break;
		case 3:
			fill[i].z = -(float)60;
			break;
		case 4:
			fill[i].z = -(float)80;
			break;
		}
	}
    fill[highlighta].x = -50;
    fill[highlighta].y = -30;
    fill[highlighta].z = 0;
    fill[highlightb].x = 50;
    fill[highlightb].y = -30;
    fill[highlightb].z = 0;
}

void getDistance(coordinates *start, coordinates *finish, coordinates *write, int size){
    for (int i = 0; i < size; i++) {
        write[i].x = (finish[i].x - start[i].x) / numinator;
        write[i].y = (finish[i].y - start[i].y) / numinator;
        write[i].z = (finish[i].z - start[i].z) / numinator;
    }
}
void changeCoordinates(coordinates *coords, coordinates *change, coordinates *newcoords){
    for (int i = 0; i < AMOUNT_OF_PEOPLE; i++){
        coords[i].x = coords[i].x + (change[i].x * count);
        coords[i].y = coords[i].y + (change[i].y * count);
        coords[i].z = coords[i].z + (change[i].z * count);
     }
    if(count >= (TOTAL_FRAMES / 2)){
        overHalf = true;
    }
    if (overHalf == false){
        count ++;
    }
    else{
        count --;
    }
    if(count <= 0){
        overHalf = false;
        count = 1;
        subtle_coords_change(&coords[0], &newcoords[0], AMOUNT_OF_PEOPLE);
        getDistance(&coords[0], &newcoords[0], &change[0], AMOUNT_OF_PEOPLE);
        animate = false;
    }

}
void initAnimations(float x, float y, float z, coordinates *coords, coordinates *tempcoords, int skip){
    if(count <= 2){
        for(int i = 0; i < AMOUNT_OF_PEOPLE; i++){
                if(i != skip){
                    tempcoords[i].x = x + (((coords[i].x - x) / 1) * count);
                    tempcoords[i].y = y + (((coords[i].y - y) / 1) * count);
                    tempcoords[i].z = z + (((coords[i].z - z) / 1) * count);
            }
        }
    }
}
#endif
