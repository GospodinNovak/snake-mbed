

float volatile speed = 0.3;

int volatile snakeLength = 11;

int volatile step = 2;

int volatile way = 0;

void joy_down_interrupt (void){
    --way;
    if (way < 0) {
        way =  3;
    }
}

void joy_up_interrupt (void){
    way = ++way % 4;
}

void initCoordinates(){
    int startX = 64;
    int startY = 16;    
    int i;
    for (i = startX; i > startX - (snakeLength * 2); i -= 2) {
        xList.append((int *)i);    
        yList.append((int *)startY);
    }
}

void ispisZmije() {
    
    int  x, y;
    node * tmp1;
    node * tmp2;
    for (int i = 1; i <= xList.length(); i++) {
        tmp1 = xList.pop(i);
        x = (int )tmp1->data;        
        tmp2 = yList.pop(i);
        y = (int )tmp2->data;        
        lcd.rect(x, y, x + 1, y + 1, 1);
    }
}

void recalculateXY (int newX, int newY, bool prolongTheSnake) {    
     
    xList.push( (int *) newX);  
    yList.push( (int *) newY); 
    
    if (!prolongTheSnake) {
        snakeLength++;
        xList.remove(xList.length());  
        yList.remove(yList.length());         
    }
}

bool checkCollision (int newX, int newY){
    int  x, y;
    node * tmp1;
    node * tmp2;
    for(int i = 1; i <= xList.length(); i++)
    {
        tmp1 = xList.pop(i);
        x = (int )tmp1->data; 
        tmp2 = yList.pop(i);
        y = (int )tmp2->data;
        if (newX == x && newY == y) {
            return true;
        }
    }
    return false;
}

void gameOver(int score){
    lcd.cls();
    lcd.locate(20, 9);
    lcd.printf("GAME OVER :( \n");
    lcd.printf("Your score is: %d" , score);
}

void generateApple(int *randX, int *randY){
    
    do {
         *randX = (rand() % 62 * 2) + 2; 
         *randY = (rand() % 14 * 2) + 2; 
    } while(checkCollision(*randX, *randY));  
}

bool checkEated(int x, int y, int nx, int ny) {
    return x == nx && ny == y;
}

int main(){
    int score = -1;
    int  xApple = 0;
    int  yApple = 2;
    srand(time(NULL));
    initCoordinates();
    
    joyINT_D.attach_deasserted(&joy_down_interrupt);
    joyINT_U.attach_deasserted(&joy_up_interrupt);
    
    joyINT_D.setSampleFrequency();
    joyINT_U.setSampleFrequency();
    
    node * tmp;    
    int x;    
    int y;
    
    bool newAppleNeeded = true;
    
    while(true) {  
    
        lcd.cls();
        if (newAppleNeeded) {
            generateApple(&xApple, &yApple);  
            score++;      
            newAppleNeeded = false;
            if (score % 5 == 0) {
                if (score > 0){
                    speed -= 0.05;
                    if (speed <= 0) {
                        speed == 0.001;
                    } 
                }
                lcd.locate(25, 9);
                lcd.printf("LEVEL %d", score / 5 + 1);
                wait (3);
                lcd.cls();                 
            }
        }
        lcd.rect(xApple, yApple, xApple + 1, yApple + 1, 1);
        ispisZmije();

        tmp = xList.pop(1);
        x = (int )tmp->data;
        tmp = yList.pop(1);
        y = (int )tmp->data;
        
        switch(way) {
            case 0:
                x += step;
                break;
            case 1:
                y += step;
                break;
            case 2:
                x -= step;
                break;
            case 3:
                y -= step;
                break;
        }
        
        x %= 128;        
        y %= 32;    
        
        x = x < 0 ? 126 : x;
        y = y < 0 ? 30 : y;
        if (checkCollision(x, y)) {
            gameOver(score);
            break;
        }
        
        recalculateXY(x, y, (newAppleNeeded = checkEated(x, y, xApple, yApple)));
     
        wait(speed);
    }
}
