class NDelayFunc  //클래스 
{
  unsigned long t_prev;         // 이전 시간  <- 변수들
  const unsigned long t_delay;  // delay 간격
  void (* func)();              // 받을 함수 주소

public:   //외부 접근 가능

  NDelayFunc(const unsigned long t_delay, void (* func)())  // 생성자
  : t_prev(0), t_delay(t_delay), func(func){}               // 변수 초기화


  void run()                          // 실행 함수
  {
    unsigned long t_now = millis();
    if(t_now - t_prev >= t_delay)
    {
      t_prev = t_now;
      
      func();                         // 함수 실행
    }
  }
};



const int LED = 10;
unsigned int t_high = 0;

void fading()                         // 함수 정의
{
  t_high++;
  if(t_high > 255) t_high = 0;
  analogWrite(LED, t_high);
}


NDelayFunc nDelayFading(4, fading);   //실행할 함수 정의 ->  클래스 함수명(변수);
void setup()
{
  
}

void loop()
{
  nDelayFading.run();  
}
