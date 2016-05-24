/*
	28/05/14
	TODO:
	
	Nested resolution, i.e. if you query build(dagon5, X), it should not give:
	
	X = [dagon4, recipe(dagon5)].
	
	It should give the full output like this:
	
	X = [staffOfWizadry, beltOfStrength, recipe(dagon1), recipe(dagon2),
		  recipe(dagon3), recipe(dagon4), recipe(dagon5)].
	
	Also, there are still issues with builds that have two or more of the same
	item, i.e. magic wand.
*/

item(empty).
% A placeholder for 'nothing'

% Consumables
item(clarity).
item(faerieFire).
item(enchantedMango).
item(tango).
item(healingSalve).
item(smokeOfDeceit).
item(townPortalScroll).
item(dustOfAppearance).
item(animalCourier).
item(flyingCourier).
item(observerWard).
item(sentryWard).
item(wards). % When both kinds of ward exist in inventory
item(tomeOfKnowledge).
item(bottle).

% Attributes
item(ironBranch).
item(gauntletsOfStrength).
item(slippersOfAgility).
item(mantleOfIntelligence).
item(circlet).
item(beltOfStrength).
item(bandOfElvenskin).
item(robeOfTheMagi).
item(ogreClub).
item(bladeOfAlacrity).
item(staffOfWizardry).

% Armaments
item(ringOfProtection).
item(stoutShield).
item(quellingBlade).
item(infusedRaindrop).
item(blightStone).
item(orbOfVenom).
item(bladesOfAttack).
item(chainmail).
item(quarterstaff).
item(helmOfIronWill).
item(broadsword).
item(claymore).
item(javelin).
item(mithrilHammer).

% Arcane
item(windLace).
item(magicStick).
item(sagesMask).
item(ringOfRegen).
item(bootsOfSpeed).
item(glovesOfHaste).
item(cloak).
item(ringOfHealth).
item(voidStone).
item(gemOfTrueSight).
item(morbidMask).
item(shadowAmulet).
item(ghostScepter).
item(blinkDagger).

% Secret shop
item(energyBooster).
item(vitalityBooster).
item(pointBooster).
item(platemail).
item(talismanOfEvasion).
item(hyperstone).
item(ultimateOrb).
item(demonEdge).
item(mysticStaff).
item(reaver).
item(eaglesong).
item(sacredRelic).

% The recipes themselves are also items
item(recipe(X)) :-
	recipe(X), !.

% As are the builds
item(X) :-
	build(X, _).

% Recipe upgrades

recipe(X) :-
	build_contains_recipe(X).

build_contains_recipe(X) :-
	build(X, Items), build_contains_recipe2(X, Items).

build_contains_recipe2(X, [recipe(X)| _T]).
build_contains_recipe2(X, [_H| T]) :-
	build_contains_recipe2(X, T), !.

% Common
build(magicWand, [magicStick, ironBranch, ironBranch, circlet]).
build(nullTalisman, [circlet, mantleOfIntelligence, recipe(nullTalisman)]).
build(wraithBand, [circlet, slippersOfAgility, recipe(wraithBand)]).
build(poorMansShield, [slippersOfAgility, slippersOfAgility, stoutShield]).
build(bracer, [circlet, gauntletsOfStrength, recipe(bracer)]).
build(soulRing, [ringOfRegen, sagesMask, recipe(soulRing)]).
build(phaseBoots, [bootsOfSpeed, bladesOfAttack, bladesOfAttack]).
build(powerTreads, [glovesOfHaste, bootsOfSpeed, beltOfStrength]).
build(powerTreads, [glovesOfHaste, bootsOfSpeed, bandOfElvenskin]).
build(powerTreads, [glovesOfHaste, bootsOfSpeed, robeOfTheMagi]).
build(oblivionStaff, [quarterstaff, sagesMask, robeOfTheMagi]).
build(perseverance, [ringOfHealth, voidStone]).
build(handOfMidas, [glovesOfHaste, recipe(handOfMidas)]).
build(bootsOfTravel, [bootsOfSpeed, recipe(bootsOfTravel)]).
build(moonShard, [hyperstone, hyperstone]).

% Support
build(ringOfBasilius, [sagesMask, ringOfProtection]).
build(ironTalon, [quellingBlade, ringOfProtection, recipe(ironTalon)]).
build(headdress, [ringOfRegen, ironBranch, recipe(headdress)]).
build(buckler, [chainmail, ironBranch, recipe(buckler)]).
build(urnOfShadows, [gauntletsOfStrength, gauntletsOfStrength, sagesMask,
      recipe(urnOfShadows)]).
build(tranquilBoots, [bootsOfSpeed, ringOfProtection, ringOfRegen]).
build(ringOfAquila, [wraithBand, ringOfBasilius]).
build(medallionOfCourage, [chainmail, sagesMask, blightStone]).
build(arcaneBoots, [bootsOfSpeed, energyBooster]).
build(drumOfEndurance, [ringOfRegen, bracer, windLace,
      recipe(drumOfEndurance)]).
build(mekansm, [headdress, buckler, recipe(mekansm)]).
build(vladimirsOffering, [headdress, ringOfBasilius, morbidMask,
      recipe(vladimirsOffering)]).
build(pipeOfInsight, [hoodOfDefiance, headdress, recipe(pipeOfInsight)]).
build(guardianGreaves, [arcaneBoots, mekansm, recipe(guardianGreaves)]).

% Caster
build(glimmerCape, [shadowAmulet, cloak]).
build(forceStaff, [staffOfWizardry, ringOfRegen, recipe(forceStaff)]).
build(veilOfDiscord, [helmOfIronWill, nullTalisman, nullTalisman,
      recipe(veilOfDiscord)]).
build(aetherLens, [energyBooster, ringOfHealth, recipe(aetherLens)]).
build(necronomicon1, [staffOfWizardry, beltOfStrength, recipe(necronomicon1)]).
build(necronomicon2, [necronomicon1, recipe(necronomicon2)]).
build(necronomicon3, [necronomicon2, recipe(necronomicon3)]).
build(dagon1, [staffOfWizardry, nullTalisman, recipe(dagon1)]).
build(dagon2, [dagon1, recipe(dagon2)]).
build(dagon3, [dagon2, recipe(dagon3)]).
build(dagon4, [dagon3, recipe(dagon4)]).
build(dagon5, [dagon4, recipe(dagon5)]).
build(eulsScepterOfDivinity, [staffOfWizardry, sagesMask, windLace,
      recipe(eulsScepterOfDivinity)]).
build(solarCrest, [medallionOfCourage, talismanOfCourage]).
build(rodOfAtos, [staffOfWizardry, staffOfWizardry, vitalityBooster]).
build(orchidMalevolence, [oblivionStaff, oblivionStaff,
      recipe(orchidMalevolence)]).
build(aghanimsScepter, [pointBooster, staffOfWizardry, ogreClub,
      bladeOfAlacrity]).
build(refresherOrb, [perseverance, perseverance, recipe(refresherOrb)]).
build(scytheOfVyse, [mysticStaff, ultimateOrb, voidStone]).
build(octarineCore, [mysticStaff, soulBooster]).

% Weapons
build(crystalys, [broadsword, bladesOfAttack, recipe(crystalys)]).
build(armletOfMordiggian, [helmOfIronWill, glovesOfHaste, bladesOfAttack,
		recipe(armletOfMordiggian)]).
build(shadowBlade, [shadowAmulet, claymore]).
build(skullBasher, [beltOfStrength, javelin, recipe(skullBasher)]).
build(battleFury, [perseverance, claymore, broadsword, quellingBlade]).
build(etherealBlade, [eaglesong, ghostScepter]).
build(silverEdge, [shadowBlade, ultimateOrb, recipe(silverEdge)]).
build(radiance, [sacredRelic, recipe(radiance)]).
build(monkeyKingBar, [demonEdge, javelin, javelin]).
build(daedalus, [crystalys, demonEdge, recipe(daedalus)]).
build(butterfly, [talismanOfEvasion, quarterstaff, eaglesong]).
build(divineRapier, [sacredRelic, demonEdge]).
build(abyssalBlade, [skullBasher, vanguard, recipe(abyssalBlade)]).
build(bloodthorn, [orchidMalevolence, crystalys, recipe(bloodthorn)]).

% Armor
build(hoodOfDefiance, [ringOfHealth, cloak, ringOfRegen]).
build(vanguard, [ringOfHealth, vitalityBooster, stoutShield]).
build(bladeMail, [broadsword, chainmail, robeOfTheMagi]).
build(soulBooster, [vitalityBooster, energyBooster, pointBooster]).
build(crimsonGuard, [vanguard, buckler, recipe(crimsonGuard)]).
build(blackKingBar, [ogreClub, mithrilHammer, recipe(blackKingBar)]).
build(lotusOrb, [perseverance, platemail, recipe(lotusOrb)]).
build(shivasGuard, [platemail, mysticStaff, recipe(shivasGuard)]).
build(bloodstone, [soulRing, soulBooster, recipe(bloodstone)]).
build(mantaStyle, [yasha, ultimateOrb, recipe(mantaStyle)]).
build(linkensSphere, [perseverance, ultimateOrb, recipe(linkensSphere)]).
build(hurricanePike, [forceStaff, dragonLance, recipe(hurricanePike)]).
build(assaultCuirass, [platemail, hyperstone, chainmail,
      recipe(assaultCuirass)]).
build(heartOfTarrasque, [reaver, vitalityBooster, recipe(heartOfTarrasque)]).

% Artifacts
build(maskOfMadness, [morbidMask, recipe(maskOfMadness)]).
build(helmOfTheDominator, [morbidMask, helmOfIronWill]).
build(dragonLance, [ogreClub, bandOfElvenskin, bandOfElvenskin]).
build(sange, [ogreClub, beltOfStrength, recipe(sange)]).
build(yasha, [bladeOfAlacrity, bandOfElvenskin, recipe(yasha)]).
build(echoSabre, [ogreClub, oblivionStaff]).
build(maelstrom, [glovesOfHaste, mithrilHammer, recipe(maelstrom)]).
build(diffusalBlade, [bladeOfAlacrity, bladeOfAlacrity, robeOfTheMagi,
		recipe(diffusalBlade)]).
build(desolator, [mithrilHammer, mithrilHammer, recipe(desolator)]).
build(heavensHalberd, [sange, talismanOfEvasion]).
build(sangeAndYasha, [sange, yasha]).
build(eyeOfSkadi, [ultimateOrb, ultimateOrb, pointBooster, orbOfVenom]).
build(mjollnir, [hyperstone, maelstrom, recipe(mjollnir)]).
build(satanic, [reaver, helmOfTheDominator, recipe(satanic)]).
