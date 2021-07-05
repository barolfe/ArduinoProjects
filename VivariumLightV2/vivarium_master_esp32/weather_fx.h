// THUNDERSTORM! 
// On the light side of things, the program can be commanded to exceute a thunderstorm, 
// where lights will dim over a duration, and to a fraction of the current solar value
// and then slowly return to the correct current solar vlaue
void randomLightning();
void lightningFlash(unsigned int row);
void thunderStorm();

void randomLightning() {
    unsigned int rand_val = random(100);
    if (rand_val == 15) {
      unsigned int rand_row = random(4);
      
      lightningFlash(rand_row);
    }
}

void lightningFlash(unsigned int row) {
      Serial.println('Lightning!');
      unsigned int _cool = cool_level_cur;
      unsigned int _warm = warm_level_cur;

      // timings alternate between on/off times, starting with on
      unsigned int cnt = 0;
      unsigned int this_delay = lightning_timings[row][cnt];

      boolean b_on = true;
      while (this_delay != 0) {
        if (b_on) {
          setLights(1023, 0);
          delay(this_delay);
        } else {
          setLights(_cool, _warm);
          delay(this_delay);
        }
        b_on = !b_on;
        cnt += 1;
        this_delay = lightning_timings[row][cnt];
      }

      setLights(_cool, _warm); // Just in case, reset the lights back to where they were
}

void thunderStorm() {

  if (b_in_thunderstorm) {
    if (b_first_thunder) {
      //thunder_interval = (thunder_duration/2/max((warm_thunder_start - warm_thunder_min, cool_thunder_start - cool_thunder_min))); // seconds between updates
      b_first_thunder = false; 
      t_thunder_start = millis();
    }

    unsigned long t_now = millis();
    if ( (t_now - t_last_thunder_check) > 500) { // based on a 10 minute target: 60*10/1000 = 600ms
        t_last_thunder_check = t_now;
        
        // using a sine profile, that we'll subtract from what the current solar values should be
       // Serial.println(t_now - t_thunder_start);
        float frac_into_storm = float(t_now - t_thunder_start) / float(thunder_duration * 1000);

        if (frac_into_storm > 1) { // We're done with the thunder storm, so bail out
          b_in_thunderstorm = false;
          return;
        }

        if ((frac_into_storm > 0.4) && (frac_into_storm < 0.65)) {
          randomLightning();
        }
        
        float sine_frac = (sin( frac_into_storm * 2*PI - PI/2 ) + 1) / 2;
        int warm_new = int( warm_level_cur_solar - (sine_frac * warm_level_cur_solar) * thunder_frac);
        int cool_new = int( cool_level_cur_solar - (sine_frac * cool_level_cur_solar) * thunder_frac);

        Serial.print("Frac into storm: ");
        Serial.print(frac_into_storm);
        Serial.print(", warm_new: ");
        Serial.print(warm_new);
        Serial.print(", cool_new: ");
        Serial.println(cool_new);
        
        setLights(cool_new, warm_new);
    }

  } else {
    b_first_thunder = true;
  }
}
