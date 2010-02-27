@REM Insure java is in your system PATH
@REM %~dp0 is where the batch lives, not sure how to do that in UNIX... 

java -cp %~dp0/uzetools.jar com.belogic.uzebox.tools.converters.midi.MidiConvert %*