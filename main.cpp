const int clk_a = 32;
const int d_a = 33;
const int rst_a = 25;
const int clk_b = 23;
const int d_b = 22;
const int rst_b = 21;
uint32_t delay_time_us = 1000;
uint8_t mat[3][3] = {
{1, 0, 0},
{0, 1, 1},
{1, 0, 1}
};
hw_timer_t * anim_timer = NULL;
portMUX_TYPE timer_mux = portMUX_INITIALIZER_UNLOCKED;
static int col = 0;
void reset_matrix() {
digitalWrite(rst_a, HIGH);
digitalWrite(rst_b, HIGH);
digitalWrite(rst_a, LOW);
digitalWrite(rst_b, LOW);
}
void input_a(uint8_t d) {
digitalWrite(d_a, !d);
digitalWrite(clk_a, HIGH);
digitalWrite(clk_a, LOW);
}
void input_b(uint8_t d) {
digitalWrite(d_b, d);
digitalWrite(clk_b, HIGH);
digitalWrite(clk_b, LOW);
}
void IRAM_ATTR animation_handler() {
portENTER_CRITICAL_ISR(&timer_mux);
int numCols = sizeof(mat[0]) / sizeof(mat[0][0]);
for(int i = numCols - 1; i >= 0; i--) {
if(i == col) {
input_b(HIGH);
} else {
input_b(LOW);
}
}
int numRows = sizeof(mat) / sizeof(mat[0]);
for(int i = numRows - 1; i >= 0; i--) {
input_a(mat[i][col]);
}
col = (col + 1) % numCols;
portEXIT_CRITICAL_ISR(&timer_mux);
}
void update_mat(uint8_t * cols, uint8_t num_rows, uint8_t num_cols) {
for(int c = 0; c < num_cols; c++) {
for(int r = 0; r < num_rows; r++) {
mat[r][c] = 0;
}
for(int r = num_rows - 1; r >= num_rows - cols[c]; r--) {
mat[r][c] = 1;
}
}
}
void setup() {
Serial.begin(115200);
pinMode(clk_a, OUTPUT);
pinMode(d_a, OUTPUT);
pinMode(rst_a, OUTPUT);
pinMode(clk_b, OUTPUT);
pinMode(d_b, OUTPUT);
pinMode(rst_b, OUTPUT);
reset_matrix();
digitalWrite(clk_a, LOW);
digitalWrite(d_a, LOW);
digitalWrite(rst_a, LOW);
digitalWrite(clk_b, LOW);
digitalWrite(d_b, LOW);
digitalWrite(rst_b, LOW);
anim_timer = timerBegin(1000000);
timerAttachInterrupt(anim_timer, &animation_handler);
timerAlarm(anim_timer, delay_time_us, true, 0);
}
void loop() {
static uint8_t cols[3];
cols[0] = 1; cols[1] = 2; cols[2] = 1;
update_mat(cols, 3, 3);
delay(1000);
cols[0] = 2; cols[1] = 1; cols[2] = 3;
update_mat(cols, 3, 3);
delay(1000);
}
