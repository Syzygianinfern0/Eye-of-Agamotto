#include <LiquidCrystal.h>
LiquidCrystal lcd(13, 12, 11, 10, 9, 8);

#define ds_col 15
#define sh_col 16
#define st_col 14
#define ds_row 17

#define start 3
#define up 2
#define down 5
#define left 4
#define right 6

char Col[21], Row[21], move_c, move_r;
int colum_data(int temp)
{
    switch (temp)
    {
    case 1:
        return 1;
        break;
    case 2:
        return 2;
        break;
    case 3:
        return 4;
        break;
    case 4:
        return 8;
        break;
    case 5:
        return 16;
        break;
    case 6:
        return 32;
        break;
    case 7:
        return 64;
        break;
    case 8:
        return 128;
        break;
    default:
        return 0;
        break;
    }
}

int row_data(int temp)
{
    switch (temp)
    {
    case 1:
        return 1;
        break;
    case 2:
        return 2;
        break;
    case 3:
        return 4;
        break;
    case 4:
        return 8;
        break;
    case 5:
        return 16;
        break;
    case 6:
        return 32;
        break;
    case 7:
        return 64;
        break;
    case 8:
        return 128;
        break;
    default:
        return 0;
        break;
    }
}

void read_button()
{
    if (!digitalRead(left))
    {
        move_r = 0;
        move_c != -1 ? move_c = -1 : move_c = 1;
        while (!digitalRead(left))
            ;
    }

    if (!digitalRead(right))
    {
        move_r = 0;
        move_c != 1 ? move_c = 1 : move_c = -1;
        while (!digitalRead(right))
            ;
    }

    if (!digitalRead(up))
    {
        move_c = 0;
        move_r != -1 ? move_r = -1 : move_r = 1;
        while (!digitalRead(up))
            ;
    }

    if (!digitalRead(down))
    {
        move_c = 0;
        move_r != 1 ? move_r = 1 : move_r = -1;
        while (!digitalRead(down))
            ;
    }
}

void show_snake(int temp)
{
    for (int n = 0; n < temp; n++)
    {
        int r, c;
        for (int k = 0; k < 21; k++)
        {
            int temp1 = Col[k];
            c = colum_data(temp1);
            int temp2 = Row[k];
            r = 0xff - row_data(temp2);
            ` for (int i = 0; i < 8; i++)
            {
                int ds = (c & 0x01);
                digitalWrite(ds_col, ds);
                ds = (r & 0x01);
                digitalWrite(ds_row, ds);
                digitalWrite(sh_col, HIGH);
                c >>= 1;
                r >>= 1;
                digitalWrite(sh_col, LOW);
            }
            digitalWrite(st_col, HIGH);
            digitalWrite(st_col, LOW);
            read_button();
            delayMicroseconds(500);
        }
    }
}

void setup()
{
    lcd.begin(16, 2);
    pinMode(ds_col, OUTPUT);
    pinMode(sh_col, OUTPUT);
    pinMode(st_col, OUTPUT);
    pinMode(ds_row, OUTPUT);
    pinMode(start, INPUT);
    pinMode(up, INPUT);
    pinMode(down, INPUT);
    pinMode(left, INPUT);
    pinMode(right, INPUT);
    digitalWrite(up, HIGH);
    digitalWrite(down, HIGH);
    digitalWrite(left, HIGH);
    digitalWrite(right, HIGH);
    digitalWrite(start, HIGH);
    lcd.setCursor(0, 0);
    lcd.print("  Snake game    ");
    lcd.setCursor(0, 1);
    lcd.print("Circuit Digest  ");
    delay(2000);
    lcd.setCursor(0, 0);
    lcd.print("   Press Start  ");
    lcd.setCursor(0, 1);
    lcd.print("     To Play    ");
    delay(2000);
}

void loop()
{
    int j, k, Speed = 40, score = 0;
    j = k = move_c = 0;
    move_r = 1;
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Score: ");
    lcd.print(score);
    while (1)
    {
        for (int i = 3; i < 21; i++)
        {
            Row[i] = 100;
            Col[i] = 100;
        }
        Row[0] = rand() % 8 + 1;
        Col[0] = rand() % 8 + 1;
        Row[1] = 1;
        Col[1] = 1;
        Row[2] = 2;
        Col[2] = 1;
        j = 2, k = 1;
        while (k == 1)
        {
            move_c = 0;
            move_r = 1;
            show_snake(1);
            lcd.setCursor(7, 0);
            lcd.print(score);
            if (!digitalRead(start))
            {
                k = 2;
                Speed = 40;
                score = 0;
            }
        }

        while (k == 2)
        {
            show_snake(Speed);
            if (Row[1] > 8 || Col[1] > 8 || Row[1] < 0 || Col[1] < 0)
            {
                Row[1] = 1;
                Col[1] = 1;
                k = 1;
                lcd.setCursor(0, 1);
                lcd.print("Game Over");
                delay(5000);
                score = 0;
                lcd.clear();
                lcd.setCursor(0, 0);
                lcd.print("Score: ");
                lcd.print(score);
            }

            if (Row[0] == Row[1] + move_r && Col[0] == Col[1] + move_c)
            {
                j++;
                Speed -= 2;
                score = score + 5;
                lcd.setCursor(7, 0);
                lcd.print(score);
                Row[0] = rand() % 8 + 1;
                Col[0] = rand() % 8 + 1;
            }

            for (int i = j; i > 1; i--)
            {
                Col[i] = Col[i - 1];
                Row[i] = Row[i - 1];
            }
            Col[1] = Col[2] + move_c;
            Row[1] = Row[2] + move_r;
        }
    }
}