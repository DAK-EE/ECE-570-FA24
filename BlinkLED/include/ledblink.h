class Blink {
      public:
        int _pin;
        int _rate;
        Blink(int pin){
            _pin = pin;
        }
        void blinkRate(int rate){
            _rate = rate;
        }  
  };