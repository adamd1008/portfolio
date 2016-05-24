# DotA :: Prolog mode

This is something I've been working on, on-and-off, for a couple of years. Prolog has always been interesting to me due to it's declarative nature, which most of us with our imperative programming jobs don't get exposed to. It's a nice departure from the ordinary, simple shenanigans of working with C/C++.

# Starting Prolog

That's easy; make sure you have SWI-Prolog installed.

From your terminal, start the interpreter with either `pl` or `swipl`.

    Welcome to SWI-Prolog (Multi-threaded, 64 bits, Version 5.7.11)
    Copyright (c) 1990-2009 University of Amsterdam.
    SWI-Prolog comes with ABSOLUTELY NO WARRANTY. This is free software,
    and you are welcome to redistribute it under certain conditions.
    Please visit http://www.swi-prolog.org for details.
    
    For help, use ?- help(Topic). or ?- apropos(Word).
    
    ?- 

Load the DotA database:

    ?- [dota].
    %  dota_items compiled 0.00 sec, 38,088 bytes
    % dota compiled 0.00 sec, 41,344 bytes
    true.
    
    ?-

# What items go into a build?

That's easy!

    ?- quick_buy(necronomicon3, X).
    X = [staffOfWizardry, beltOfStrength, recipe(necronomicon1), recipe(necronomicon2), recipe(necronomicon3)].
    
    ?-

# Dev notes

There are bugs in this; for example:

    ?- quick_buy(dagon5, X).
    X = [staffOfWizardry, circlet, mantleOfIntelligence, recipe(nullTalisman), recipe(dagon1), recipe(dagon2), recipe(dagon3), recipe(dagon4), recipe(...)].

Note the `...'

The database definition is the same as for the Necronomicon item AFAICT:

    ?- quick_buy(necronomicon3, X).
    X = [staffOfWizardry, beltOfStrength, recipe(necronomicon1), recipe(necronomicon2), recipe(necronomicon3)].
