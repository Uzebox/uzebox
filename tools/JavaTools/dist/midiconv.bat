@REM Insure java is in your system PATH
@REM %~dp0 is where the batch lives, not sure how to do that in UNIX... 
@REM Version 1.1: fixes to loop handling
@REM See http://uzebox.org/wiki/index.php?title=Sound_Engine for usage
java -cp %~dp0/uzetools.jar com.belogic.uzebox.tools.converters.midi.MidiConvert %*