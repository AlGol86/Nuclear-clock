//init value settings 
#define YEAR_INIT   21
#define MONTH_INIT  11
#define DATE_INIT   13
#define WEEK_INIT   6
#define HR_INIT  21
#define MIN_INIT    53

#define SECOND_PER_DAY  86400
#define SECOND_MOSCOW_ALIGNMENT  (3*3600)
#define FIRST_YEAR      1970
#define LEAP_DAYS       366
#define NON_LEAP_DAYS   365

#define PIN_dat_rtc     4   //port C
#define PIN_clk_rtc     5   //port C
#define PIN_rst_rtc     3   //port A

#define BIT_dat_rtc    (1<<PIN_dat_rtc)
#define BIT_clk_rtc    (1<<PIN_clk_rtc)
#define BIT_rst_rtc    (1<<PIN_rst_rtc)


#define SEC   0
#define MIN   1
#define HR    2
#define DATE  3
#define MONTH 4
#define DAY   5
#define YEAR  6
#define CONTR 7
#define TR_CH 8
#define WHOLE 3

typedef struct {
  int year;
  char month;
  char dayOfMonth;
} LocalDate; 

typedef struct {
  char hr;
  char min;
  char sec;
} LocalTime; 

typedef struct {
  unsigned long epochSecFirstPoint;
  long shiftMillis;
  long correctionMillis;
} TimeAlignment; 

void delay_rtc(char del );
void rtc_set_time(void);
void init_rtc(void);
void send_byte_rtc(char data);
char receive_byte_rtc(void);
void send_rtc(char reg_adr, char data);
char receive_rtc(char reg_adr);
char receive_plain_val_rtc(char reg_adr);
void print_time(void);
void print_time_giga(void);
void init_tim1(void);
char equals(LocalDate date1, LocalDate date2);
char getMonthLength(char month, char leap);
char isLeapYear(int year);
LocalDate getMonth(int days, int year);
LocalDate getDate(long days);
unsigned long getDaysOfDate(LocalDate date);
unsigned long getDays(unsigned long seconds);
unsigned long receive_seconds_rtc_utc();