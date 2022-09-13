const int NUM_FADERS = 4;
uint16_t smoothed[NUM_FADERS] = {512,512,512,512};
int mutes[NUM_FADERS] = {6,5,4,3};

void setup() {
  analogReference(INTERNAL1V1);

  ADC0.CTRLC = ADC_PRESC_DIV16_gc      /* CLK_PER divided by 16 */
              | ADC_REFSEL_INTREF_gc;  /* Internal reference */

  ADC0.CTRLA = ADC_ENABLE_bm         /* ADC Enable: enabled */
              | ADC_RESSEL_10BIT_gc;   /* 10-bit mode */


  /* Set the accumulator mode to accumulate 64 samples */
  ADC0.CTRLB = ADC_SAMPNUM_ACC64_gc;

  // for (int i = 0; i < NUM_FADERS; i++) {
  //   pinMode(mutes[i], INPUT);
  // }

  attachInterrupt(digitalPinToInterrupt(3), next_page, RISING);


  Serial.begin(115200);
}

void loop() {
  for (int i = 0x00; i < NUM_FADERS; i++) {
    /* Select ADC channel */
    ADC0.MUXPOS  = i;

    uint16_t adcVal = ADC0_read();

    /* divide by 64 */
    adcVal = adcVal >> 6;

    /* average with previous results, 50/50 */
    smoothed[i] = (smoothed[i] >> 1) + (adcVal >> 1);

    Serial.print("$" + String(i) + "." + String(smoothed[i]) + "." + String(digitalRead(i)) + "%");
  }
  Serial.write("\n");
}

uint16_t ADC0_read(void)
{
    /* Start ADC conversion */
    ADC0.COMMAND = ADC_STCONV_bm;

    /* Wait until ADC conversion done */
    while ( !(ADC0.INTFLAGS & ADC_RESRDY_bm) )
    {
        ;
    }

    /* Clear the interrupt flag by writing 1: */
    ADC0.INTFLAGS = ADC_RESRDY_bm;

    return ADC0.RES;
}

void next_page() {
  Serial.flush();
  Serial.print("$1%");
}

void last_page() {
  Serial.flush();
  Serial.print("$0%");
}