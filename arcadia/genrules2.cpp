// START A3HEADER
//
// This source file is part of the Atlantis PBM game program.
// Copyright (C) 1995-1999 Geoff Dunbar
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program, in the file license.txt. If not, write
// to the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
// Boston, MA 02111-1307, USA.
//
// See the Atlantis Project web page for details:
// http://www.prankster.com/project
//
// END A3HEADER

#include <time.h>

#include "game.h"
#include "gamedata.h"
#include "fileio.h"

AString Game::NumToWord(int n)
{
	if(n > 20) return AString(n);
	switch(n) {
		case  0: return AString("zero");
		case  1: return AString("one");
		case  2: return AString("two");
		case  3: return AString("three");
		case  4: return AString("four");
		case  5: return AString("five");
		case  6: return AString("six");
		case  7: return AString("seven");
		case  8: return AString("eight");
		case  9: return AString("nine");
		case 10: return AString("ten");
		case 11: return AString("eleven");
		case 12: return AString("twelve");
		case 13: return AString("thirteen");
		case 14: return AString("fourteen");
		case 15: return AString("fifteen");
		case 16: return AString("sixteen");
		case 17: return AString("seventeen");
		case 18: return AString("eighteen");
		case 19: return AString("nineteen");
		case 20: return AString("twenty");
	}
	return AString("error");
}

int Game::GenRules2(Arules f, int app_exist, int qm_exist, int move_over_water, int may_sail)
{
    
	AString temp, temp2;
	int cap;
	int i, j, k, l;
	int last = -1;
	AString skname;
	SkillType *pS;

	int has_stea = !(SkillDefs[S_STEALTH].flags & SkillType::DISABLED);
	int has_obse = !(SkillDefs[S_OBSERVATION].flags & SkillType::DISABLED);
	int comma = 0;
	int first = 0;
	
	
	f.LinkRef("com_muster");
	f.TagText("h3", "The Muster:");
	temp = "Once the attack has been made, the sides are gathered.  Although "
		"the ";
	temp += f.Link("#attack", "ATTACK") + " order takes a unit rather than "
		"a faction as its parameter (mainly so that unidentified units can "
		"be attacked), an attack is basically considered to be by an entire "
		"faction, against an entire faction and its allies.";
	f.Paragraph(temp);
	temp = "On the attacking side are all units of the attacking faction in "
		"the region where the fight is taking place, except those with Avoid "
		"Combat set.  A unit which has explicitly (or implicitly via ";
	temp += f.Link("#advance", "ADVANCE") + ") issued an " +
		f.Link("#attack", "ATTACK") + " order will join the fight anyway, "
		"regardless of whether Avoid Combat is set.";
	f.Paragraph(temp);
	temp = "Also on the attacking side are all units of other factions that "
		"attacked the target faction (implicitly or explicitly) in the "
		"region where the fight is taking place.  In other words, if several "
		"factions attack one, then all their armies join together to attack "
		"at the same time (even if they are enemies and will later fight "
		"each other).";
	f.Paragraph(temp);
	temp = "On the defending side are all identifiable units belonging to "
		"the defending faction.  If a unit has Avoid Combat set and it "
		"belongs to the target faction, it will be uninvolved only if its "
		"faction cannot be identified by the attacking faction.  A unit "
		"which was explicitly attacked will be involved anyway, regardless "
		"of Avoid Combat. ";
	if(has_stea) {
		temp += "(This means that Avoid Combat is mostly useful for high "
			"stealth scouts.) ";
	}
	temp += "Also, all non-avoiding units located in the target region "
		"belonging to factions allied with the defending unit will join "
		"in on the defending side";
	if(Globals->ALLIES_NOAID)
		temp += ", provided that at least one of the units belonging to "
		    "the defending faction is not set to noaid.";
	else
		temp += ".";
	f.Paragraph(temp);
	temp = "Units in adjacent regions can also become involved.  This is "
		"the exception to the general rule that you cannot interact with "
		"units in a different region.";
	f.Paragraph(temp);
	temp = "If a faction has at least one unit involved in the initial "
		"region, then any units in adjacent regions will join the fight, "
		"if they could reach the region and do not have Avoid Combat set. "
		"There are a few flags that units may set to affect this; a unit "
		"with the Hold flag (set using the ";
	temp += f.Link("#hold", "HOLD") + " order) will not join battles in "
		"adjacent regions.  This flag applies to both attacking and "
		"defending factions.  A unit with the Noaid flag (set using the ";
	temp += f.Link("#noaid", "NOAID") + " order) will receive no aid from "
		"adjacent hexes when attacked, or when it issues an attack.";
	f.Paragraph(temp);
	temp = "Example:  A fight starts in region A, in the initial combat "
		"phase (before any movement has occurred).  The defender has a unit "
		"of soldiers in adjacent region B.  They have 2 movement points at "
		"this stage. ";
	temp += "They will buy horses later in the turn, so that when "
		"they execute their ";
	temp += f.Link("#move", "MOVE") + " order they will have 4 movement "
		"points, but right now they have 2. ";
	if(Globals->WEATHER_EXISTS)
		temp += "Region A is forest, but fortunately it is summer, ";
	else
		temp += "Region A is forest, ";
	temp += "so the soldiers can join the fight.";
	f.Paragraph(temp);
	temp = "It is important to note that the units in nearby regions do not "
		"actually move to the region where the fighting happens; the "
		"computer only checks that they could move there.  (In game world "
		"terms, presumably they did move there to join the fight, and then "
		"moved back where they started.)  The computer checks for weight "
		"allowances and terrain types when determining whether a unit could "
		"reach the scene of the battle. Note that the use of ships is not "
		"allowed in this virtual movement.";
	f.Paragraph(temp);
	temp = "If you order an attack on an ally (either with the ";
	temp += f.Link("#attack", "ATTACK") + " order, or if your ally has "
		"declared you Unfriendly, by attempting to ";
	temp += f.Link("#advance", "ADVANCE") +" into a region which he is "
		"guarding), then your commander will decide that a mistake has "
		"occurred somewhere, and withdraw your troops from the fighting "
		"altogether.  Thus, your units will not attack that faction in "
		"that region. Note that you will always defend an ally against "
		"attack, even if it means that you fight against other factions "
		"that you are allied with.";
	f.Paragraph(temp);
	f.LinkRef("com_thebattle");
	f.TagText("h3", "The Battle:");
	
	if(!(SkillDefs[S_TACTICS].flags & SkillType::DISABLED)) {
		temp = " The computer selects the best tactician from each side; "
			"that unit is regarded as the commander of its side.  If two or "
			"more units on one side have the same Tactics skill, then the "
			"one with the lower unit number is regarded as the commander of "
			"that side.";
	f.Paragraph(temp);			
	}
	
	temp = "Having arrived on the battlefield, the troops are split up into battle formations.";
	temp += " Formations can be of three types - foot, riding, or flying. At the start "
	        "of each battle, two formations of each type will be created, one for units "
	        "which will be fighting from the front, and one for units with the behind flag "
	        "activated, which will attempt to avoid hand-to-hand combat. To join a riding "
            "formation, a soldier must not only have a mount, but to have high enough "
            "riding skill to use that mount in combat. To join a flying formation, the soldier "
            "must have a mount capable of flight, and sufficient skill to use it.";
    temp += " The terrain in which the battle is fought has a strong influence on the battle "
            "lines. The following table presents the effects of various terrains on foot, "
            "riding and flying units.";
	f.Paragraph(temp);
            
		f.LinkRef("tablebattleterrain");
		f.Enclose(1, "center");
		f.Enclose(1, "table border=\"1\"");
		f.Enclose(1, "tr");
		f.TagText("td", "Terrain");
		f.TagText("th", " Foot ");
		f.TagText("th", " Riding ");
		f.TagText("th", " Flying ");
		f.TagText("th", " Ranged ");		
		f.Enclose(0, "tr");
		int num = 8;
		if(Globals->UNDERWORLD_LEVELS > 0) num = 11;
    	if(Globals->UNDERDEEP_LEVELS > 0) num = 14;
		
		
		for(i = 0; i < num; i++) {
//			if(RegionDefs[i].flags & RegionDefs::DISABLED) continue;

			/* Okay, this is a valid region for standard games! */
			f.Enclose(1, "tr");
			f.Enclose(1, "td align=\"left\" nowrap");
			f.PutStr(TerrainDefs[i].name);
			f.Enclose(0, "td");
			f.Enclose(1, "td align=\"center\"");	
			if(TerrainDefs[i].flags & TerrainType::RESTRICTEDFOOT) temp = "   Restricted   ";
			else temp = "   Normal   ";
			f.PutStr(temp);
			f.Enclose(0, "td");
			f.Enclose(1, "td align=\"center\"");	
			if(TerrainDefs[i].flags & TerrainType::RIDINGLIMITED) temp = "   Limited   ";
			else if(TerrainDefs[i].flags & TerrainType::RIDINGMOUNTS) temp = "   Normal   ";
			else temp = "   None   ";
			f.PutStr(temp);
			f.Enclose(0, "td");
			f.Enclose(1, "td align=\"center\"");	
			if(TerrainDefs[i].flags & TerrainType::FLYINGLIMITED) temp = "   Limited   ";
			else if(TerrainDefs[i].flags & TerrainType::FLYINGMOUNTS) temp = "   Normal   ";
			else temp = "   None   ";
			f.PutStr(temp);		
			f.Enclose(0, "td");
			f.Enclose(1, "td align=\"center\"");	
			if(TerrainDefs[i].flags & TerrainType::RESTRICTEDRANGED) temp = "   Restricted   ";
			else temp = "   Normal   ";
			f.PutStr(temp);
			f.Enclose(0, "td");
			f.Enclose(0, "tr");
		}
		f.Enclose(0, "table");
		f.Enclose(0, "center");
            
	temp = "If the terrain does not allow for riding in battle, "
           "then any units which would normally ride will fight on foot. "
           "If flying units are not allowed in the terrain, then those "
           "units will try to ride. If they cannot ride, they "
	       "will also fight on foot. If the terrain allows for limited "
           "riding or flying, then formations will be created and behave "
           "as normal; however, the soldiers in these formations will "
           "not get any combat bonus due to their riding skill, as "
           "detailed later in this section. In terrains where foot "
           "movement is restricted, foot formations may not undertake "
           "any battle maneuvres, such as flanking around outnumbered "
           "opponents. If ranged attacks are restricted, "
           "ranged units have a -1 penalty to their attack skill "
           "and chance-to-attack. ";
	f.Paragraph(temp); 
 
	temp = "Formations are also limited by the skill of the tactitian "
           "commanding the battle. If the commander of a side is not "
           "sufficiently skilled to command "
           "flying or riding cavalry, then those units will fight "
           "using tactics understood by the commander. However, if the "
           "terrain allows it, these units will still get a combat "
           "bonus from their riding skill.";
	f.Paragraph(temp); 
                                   
            /*
	if(!(SkillDefs[S_TACTICS].flags & SkillType::DISABLED)) {
		temp += " The computer selects the best tactician from each side; "
			"that unit is regarded as the leader of its side.  If two or "
			"more units on one side have the same Tactics skill, then the "
			"one with the lower unit number is regarded as the leader of "
			"that side.  If one side's leader has a better Tactics skill "
			"than the other side's, then that side gets a free round of "
			"attacks.";
	}
	f.Paragraph(temp);*/
	temp = "Coordinating large armies is no easy task. Although the commander of "
           "an army directs it during battle, large armies must have sufficient "
           "soldiers skilled in tactics ('officers') to convey his orders to "
           "the entire army. A soldier with tactics 1 is capable of directing "
           "ten soldiers, including himself, in battle. At higher levels of tactics, "
           "officers are able to coordinate ever larger groups of men. "
           "If there are not enough officers (including the commander) present to coordinate an army, "
           "then some soldiers will not be placed in their proper formation. In "
           "addition, a lack of coordination will effect the success of "
           "some maneuvers undertaken during battle. "
           "Some magic spells can decrease the efficiency of officers in armies, "
           "potentially turning a well commanded army into a confused horde of soldiers.";
	f.Paragraph(temp);
	
	temp = AString("In each combat round, combat formations vie for position. ") +
           "Foot troops will initially engage the enemy foot troops, "
           "while cavalry may attempt to flank behind the enemy, or "
           "stay in reserve and defend ranged troops from "
           "enemy cavalry. Formations which sufficiently outnumber "
           "their opponents may also send some men to flank "
           "behind the enemy; if both sides have sufficient officers "
           "then a formation needs to be 2.5 times the size of its "
           "before any men will flank. Most of these decisions are "
           "made by the commander of the army, but they can be "
           "influenced with the " + f.Link("#tactics", "TACTICS") + " order "
           "which can make the commander more or less aggressive in the use "
           "of his cavalry.";
    f.Paragraph(temp);
           
	temp = "Units which successfully flank behind the enemy may hit "
           "ranged soldiers who are behind the frontline, or attack the "
           "enemy frontline from behind. In the latter case, they gain a +1 bonus "
           "to their attack skill and chance-to-attack.";
	temp += " Cavalry and aerial cavalry units which are positioned "
	       "behind the frontline (eg mounted archers) will attempt to evade their attackers, "
           "with flying units more likely to succeed in doing so. "
           "However, ranged units do not need to "
           "be able to catch a formation in order to attack them.";
	f.Paragraph(temp);
	
	temp = "When formations have completed their positioning, the "
        "combatants each get to attack once, in a random order. "
        "Each combatant will attempt to hit an enemy randomly "
        "selected from all formations with which he is engaged. "
		"If he hits, and the target has no armour, then the target is "
		"automatically killed.  Armour may provide extra defense against "
		"otherwise successful attacks. "        
        "If the attacker's formation is not directly engaged with "
        "the enemy (for example "
        "a soldier with the behind flag which has not been "
        "engaged by enemy cavalry) then he will only attack if "
        "equipped with a ranged weapon or magic skill. Ranged units "
        "will first target formations they are personally engaged with, "
        "then formations which have flanked or are flanking, and then "
        "the enemy's frontline. Only when all other enemies have been "
        "killed will a ranged unit with the behind flag attack enemy "
        "ranged formations. ";
    temp += "If any melee formation kills all enemies with which it is engaged, then "
        "it will attempt to engage another target.";
	f.Paragraph(temp);
	
	temp = "The basic skill used in battle is the Combat skill; this is "
		"used for hand to hand fighting.  If one soldier tries to hit "
		"another using most weapons, he has a 1:1 (ie 50%) chance-to-attack. "
        "This chance-"
        "to-attack may be modified by terrain (-1 for ranged units in "
        "restricted terrains), by position (+1 for soldiers who have "
        "attacked the enemy frontline from behind while the enemy is still "
        "engaged with attackers from the front), by fortifications (-1 "
        "if attacking a soldier in a fort or other defensive "
        "building), or by some spells. A +1 bonus gives a soldier a 1+1=2:1 "
        "(i.e. 67%) chance-to-attack, while a -2 bonus gives a soldier "
        "a 1:1+2=3 (ie 25%) chance. If the "
		"attacker does get to attack, then there is a contest "
		"between his combat skill (modified by weapon attack bonus) and "
		"the defender's combat skill (modified by weapon defense bonus). "
		"Some weapons may not allow combat skill to affect defense (e.g. "
		"bows), and others may allow different skills to be used on "
		"defense (or offense).";
	f.Paragraph(temp);
	temp = "If the skills are equal, then there is a 1:1 (i.e. 50%) "
		"chance that the attack will succeed.  If the attacker's skill is 1 "
		"higher then there is a 2:1 (i.e. 67%) chance, if the attacker's "
		"skill is 2 higher then there is a 4:1 (i.e. 80%) chance, 3 higher "
		"means an 8:1 (i.e. 88%) chance (2*2*2 = 8), and so on. Similarly if the "
		"defender's skill is 3 higher, then there is only a 1:8 (i.e. 11%) "
		"chance, etc.";
	f.Paragraph(temp);
	temp = "";
	temp = "There are a variety of weapons in the world which can increase "
		"a soldier's skill on attack or defense.  Better weapons will "
		"generally convey better bonuses, but not all weapons are as good "
		"in all situations. Specifics about the bonuses conferred by "
		"specific weapons can be found both in these rules (for most basic "
		"weapons), and in the descriptions of the weapons themselves. Troops "
		"which are fighting hand-to-hand without specific weapons are "
		"assumed to be irregularly armed with makeshift weapons such as "
		"clubs, pitchforks, torches, etc. ";
	f.Paragraph(temp);

	temp = " Possession of a mount, and the appropriate skill to use that "
		"mount, may also confer a bonus to the effective combat skill. The "
		"amount of the bonus will depend on the level of the appropriate "
		"skill and the mount in question. Some mounts are better than "
		"others, and may provide better bonus, but may also require higher "
		"levels of skill to get any bonus at all. Some terrain, as listed "
        "above, will not "
		"allow mounts to give a combat advantage at all.";
	f.Paragraph(temp);

/*   Not enabled in Arcadia !!!
	temp += "Certain weapons may provide different attack and defense "
		"bonuses, or have additional attack bonuses against mounted "
		"opponents or other special characteristics. These bonuses will "
		"be listed in the item descriptions in the turn reports.";
	f.Paragraph(temp);
	temp = "Some melee weapons may be defined as Long or Short (this is "
		"relative to a normal weapon, e.g. the sword).  A soldier wielding "
		"a longer weapon than his opponent gets a +1 bonus to his attack "
		"skill.";
	f.Paragraph(temp);
	*/
	temp = "Ranged weapons are slightly different from melee weapons.  The "
		"target will generally not get any sort of combat bonus to defense "
		"against a ranged attack. Also, the weapon may require a skill other "
        "than the Combat skill for a soldier to use. "
        "In addition, when defending against an attack, a "
        "soldier using a ranged weapon will generally be "
		"treated as if they have a Combat skill of 0, even if they have an "
		"actual Combat skill.  This is the trade off for being able to hit "
		"from the back line of fighting.";
	f.Paragraph(temp);
	temp = "Some weapons, including some ranged weapons, may only attack "
		"every other round, or even less frequently. When a weapon is not "
		"able to attack every round, this will be specified in the item "
		"description. Wielders of these weapons will still move around in "
        "their formations, even if they cannot attack in that round.";
	f.Paragraph(temp);
	temp = "Weapons may have one of several different attack types: "
		"Slashing, Piercing, Crushing, Cleaving and Armour Piercing.  "
		"Different types of armour may give different survival chances "
		"against a sucessful attack of different types.";
		//Arcadia only:
    temp += " In this version of Atlantis, Slashing, Piercing, "
        "Crushing and Cleaving weapons all have identical effects, "
        "and there is no difference between these types of attacks. "
        "The terminology is included only because Atlantis "
        "based on a game engine which has the ability to be more "
        "more complex.";
	f.Paragraph(temp);
	temp = "Being inside a building confers a bonus to a soldier's defence "
        "skill (in addition to giving the attacker a -1 penalty to his "
        "chance-to-attack). This "
		"bonus is effective against ranged as well as melee attacks. "
/*        "The magnitude of the defensive bonus may vary between buildings, "
        "and will be listed in the description of a building. "*/
        "This bonus is equal to a +2 increase in the defensive skill level, "
        "regardless of the building type, providing that the number of men "
        "the building can protect is not exceeded (some buildings, such as "
        "trade structures, cannot protect any men). "
		"The number of men that a building can protect is equal to its size. "
		"The size of the various common buildings was listed in the ";
	temp += f.Link("#tablebuildings", "Table of Buildings") + " earlier. ";
	f.Paragraph(temp);
	temp = "If there are too many units in a building to all gain "
		"protection from it, then those units who have been in the building "
		"longest will gain protection.  (Note that these units appear first "
		"on the turn report.)";
	if(!(ObjectDefs[O_FORT].flags & ObjectType::DISABLED)) {
		temp += " If a unit of 200 men is inside a Fort (capacity ";
		temp += ObjectDefs[O_FORT].protect;
		temp += "), then the first ";
		temp += ObjectDefs[O_FORT].protect;
		temp += " men in the unit will gain the full +2 bonus, and the other ";
		temp += (200 - ObjectDefs[O_FORT].protect);
		temp += " will gain no protection.";
	}
	f.Paragraph(temp);
	
	f.LinkRef("com_report");
	f.TagText("h3", "The Battle Report");
	temp = "Everything which occurs during a battle will be recorded in the battle "
	    "report. This is seen by everyone who has units in the region where the "
	    "battle took place. For each round of the battle, you will see a list of "
	    "spells cast, formation movements, and a summary of how many people from "
        "each side died in that combat round. There will also be a message if one "
        "army does not have enough officers for full control of their soldiers, allowing that player "
        "to remedy the situation in time for their next battle.";
	f.Paragraph(temp);
	temp = "In addition, at the start of every combat round, you will see a diagrammatical "
        "map of the battle, marking infantry, cavalry, aerial cavalry and ranged troops "
        "from both sides. An example map is below: ";
	f.Paragraph(temp);
	
	f.Enclose(1, "pre");
	f.PutNoFormat("            The Battle Position:");
	f.PutNoFormat("                           Fighters (106)");
	f.PutNoFormat("                               R#: 5                 - line 1");
	f.PutNoFormat("                      C#: 20   I*: 12                - line 2");
	f.PutNoFormat("                      C#: 18   I#: 36                - line 3");
	f.PutNoFormat("                               I*: 102               - line 4");
	f.PutNoFormat("                      C*: 5                          - line 5");
	f.PutNoFormat("                      C#: 43   R*: 4    A#: 18       - line 6");
	f.PutNoFormat("                           City Guard (8)");
	f.Enclose(0, "pre");

	temp = "In this example, unit 106, named 'Fighters', attacked unit 8, the City Guard. "
        "Entries marked with a # represent soldiers belonging to the attacker, 'Fighters', "
        "while entries with a * represent soldiers belonging to the defender, 'City Guard'. "
        "The first line below fighters has places for three entries. First, are listed the "
        "number of 'City Guard' cavalry which have flanked around to 'Fighters' backline. "
        "Second, is listed the number of troops in 'Fighters' ranged formations, whether "
        "on foot, horseback, or flying. Third, are the number of aerial cavalry belonging "
        "to 'City Guard' which have flanked around to 'Fighters' backline. In this instance, "
        "two of these entries are blank, indicating that 'City Guard' does not have any "
        "cavalry behind 'Fighters' frontline. 'Fighters' does, however, have 5 troops "
        "assigned to his ranged formations. These are soldiers from a unit with the behind "
        "flag set, they may or may not have ranged weapons.";
	f.Paragraph(temp);
	temp = "The second line in this example lists: (i) 'Fighters' cavalry which is in reserve, "
        "and trying to prevent 'City Guard' soldiers from getting to the 'Fighters' backline, "
        "(ii) 'City Guard' infantry which has flanked around 'Fighters' frontline, and reached "
        "their backline, and (iii) 'Fighters' aerial cavalry which is in reserve. In this case, 'City Guard' "
        "has 12 infantry which reached the backline of 'Fighters', despite 'Fighters' still "
        "having 20 cavalry in reserve. Such a situation would not commonly occur (usually the "
        "cavalry would intercept the soldiers before they reached the backline) but there "
        "are some spells which may bring this about. The map does not tell you who 'City Guards' "
        "flanked infantry is fighting, but in most cases it will be attacking the 'Fighters' 5 ranged "
        "soldiers, while also being attacked by 'Fighters' 20 reserve cavalry. "
        "The third line in the battle lists (i) 'Fighters' cavalry which is trying to reach "
        "the 'City Guard' backline, but have been prevented from doing by the 5 'City Guard' cavalry "
        "in reserve (listed at the start of line 5). (ii) is 'Fighters' infantry, which is either in the battle "
        "frontline, or attempting to flank to the 'City Guard' backline, and (iii) 'Fighters' "
        "aerial cavalry which has been blocked by 'City Guard' reserves (there are none in "
        "this battle). ";
	f.Paragraph(temp);
    temp = "Lines 4, 5 and 6 are the mirror image of lines 3, 2 and 1, with the "
        "sides swopped. Thus, in this battle, 'City Guard' had little cavalry, and was unable to "
        "prevent most of the 'Fighters' cavalry - and all of his aerial cavalry - from reaching the 'City Guard' backline and attacking his "
        "ranged soldiers, of which 4 survive. 'City Guard' did manage to get 12 infantry past "
        "the outnumbered frontline of 'Fighters', despite 'Fighters' having cavalry kept in reserve to "
        "prevent such a move. However, that infantry is likely to die quickly, under attack from "
        "20 cavalry and 5 ranged troops. 'Fighters' might try for the next battle to have more "
        "troops in his frontline so that the 'City Guard' infantry is tied down unable to flank, "
        "while 'City Guard' would need to bring more cavalry to defend his ranged troops, or "
        "alternatively mix them in with his melee infantry, so that they may not be picked off "
        "by 'Fighter's large number of cavalry.";
	f.Paragraph(temp);

	f.LinkRef("com_victory");
	f.TagText("h3", "Victory!");
	temp = "Combat rounds continue until one side has accrued 50% losses "
		"(or more). Illusions are not counted in this calculation.";
	if(Globals->ARMY_ROUT != GameDefs::ARMY_ROUT_FIGURES) temp += " Soldiers "
	    "with multiple hit points are counted (hitpoints) times in this "
	    "calculation, and each time are counted as alive until the "
	    "soldier dies (loses his last hitpoint).";
    temp += " There is then one more round of attacks with the "
        "losing side suffering "
        "a -1 penalty to their chance-to-hit and attack skill. If both sides have "
		"more than 50% losses, the battle is a draw, and there is no "
        "extra round.";
	f.Paragraph(temp);
	if(!(SkillDefs[S_HEALING].flags & SkillType::DISABLED) &&
			!(ItemDefs[I_HERBS].flags & SkillType::DISABLED)) {
		temp = "Units with the Healing skill have a chance of being able "
			"to heal casualties of the winning side, so that they recover "
			"rather than dying. Each character with this skill can attempt "
			"to heal ";
		temp += Globals->HEALS_PER_MAN;
		temp += AString(" casualties per skill level. Each attempt however "
			"requires one unit of Herbs, which is thereby used up. Each "
			"attempt has a ") + HealDefs[1].rate + "% chance of healing one casualty; only one "
			"attempt at Healing may be made per casualty. Healing occurs "
			"automatically, after the battle is over, by any living "
			"healers on the winning side.";
		f.Paragraph(temp);
	}
	temp = "Any items owned by dead combatants on the losing side have a "
		"50% chance of being found and collected by the winning side. "
		"Each item which is recovered is picked up by one of the "
		"survivors able to carry it (see the ";
	temp += f.Link("#spoils", "SPOILS") + " command) at random, so the "
		"winners generally collect loot in proportion to their number of "
		"surviving men.";
	f.Paragraph(temp);
	temp = "If you are expecting to fight an enemy who is carrying so "
		"much equipment that you would not be able to move after picking "
		"it up, and you want to move to another region later that month, it "
		"may be worth issuing some orders to drop items (with the ";
	temp += f.Link("#give", "GIVE") + " 0 order) or to prevent yourself "
		"picking up certain types of spoils (with the ";
	temp += f.Link("#spoils", "SPOILS") + " order) in case you win the "
		"battle! Also, note that if the winning side took any losses in "
		"the battle, any units on this side will not be allowed to move, "
		"or attack again for the rest of the turn. Units on the losing "
        "side do not suffer this penalty.";
	f.Paragraph(temp);
	if(has_stea || has_obse) {
		f.LinkRef("stealthobs");
		f.ClassTagText("div", "rule", "");
		temp = (has_stea ? "Stealth" : "");
		if(has_obse) {
			if(has_stea) temp += " and ";
			temp += "Observation";
		}
		f.TagText("h2", temp);
		if(has_stea && has_obse) {
			temp = "The Stealth skill is used to hide units, while the "
				"Observation skill is used to see units that would otherwise "
				"be hidden. A unit can be seen only if you have at least "
				"one unit in the same region, with an Observation skill at "
				"least as high as that unit's Stealth skill. If your "
				"Observation skill is equal to the unit's Stealth skill, "
				"you will see the unit, but not the name of the owning "
				"faction. If your Observation skill is higher than the "
				"unit's Stealth skill, you will also see the name of the "
				"faction that owns the unit.";
		} else if(has_stea) {
			temp = "The Stealth skill is used to hide units. A unit can be "
				"seen only if it doesn't know the Stealth skill and if you "
				"have at least one unit in the same region.";
		} else if(has_obse) {
			temp = "The Observation skill is used to see information about "
				"units that would otherwise be hidden.  If your unit knows "
				"the Observation skill, it will see the name of the faction "
				"that owns any unit in the same region.";
		}
		f.Paragraph(temp);
		if(has_stea) {
			temp = "Regardless of Stealth skill, units are always visible "
				"when participating in combat; when guarding a region with "
				"the Guard flag; or when in a building or aboard a ship.";
			if(has_obse) {
				temp += " However, in order to see the faction that owns "
					"the unit, you will still need a higher Observation "
					"skill than the unit's Stealth skill.";
			}
			f.Paragraph(temp);
			f.LinkRef("stealthobs_stealing");
			f.TagText("h3", "Stealing:");
			temp = AString("The ") + f.Link("#steal", "STEAL") +
				" order is a way to steal items from other factions without "
				"a battle.  The order can only be issued by a one-man unit. "
				"The order specifies a target unit; the thief will then "
				"attempt to steal the specified item from the target unit.";
			f.Paragraph(temp);
			if(has_obse) {
				temp = "If the thief has higher Stealth than any of the "
					"target faction's units have Observation (i.e. the "
					"thief cannot be seen by the target faction), the theft "
					"will succeed.";
			} else {
				temp = "The thief must know Stealth to attempt theft.";
			}
			temp += " The target faction will be told what was stolen, but "
				"not by whom.  If the specified item is silver, then $200 "
				"or half the total available, whichever is less, will be "
				"stolen.  If it is any other item, then only one will be "
				"stolen (if available).";
			f.Paragraph(temp);
			if(has_obse) {
				temp = "Any unit with high enough Observation to see the "
					"thief will see the attempt to steal, whether the "
					"attempt is successful or not.  Allies of the target "
					"unit will prevent the theft, if they have high enough "
					"Observation to see the unit trying to steal. Non-player-"
                    "factions, such as City Guards, may not be stolen from.";
				f.Paragraph(temp);
			}
			f.LinkRef("stealthobs_assassination");
			f.TagText("h3", "Assassination:");
			temp = AString("The ") + f.Link("#assassinate", "ASSASSINATE") +
				" order is a way to kill another person without attacking "
				"and going through an entire battle. This order can only be "
				"issued by a one-man unit, and specifies a target unit.  If "
				"the target unit contains more than one person, then one "
				"will be singled out at random.";
			f.Paragraph(temp);
			if(has_obse) {
				temp = "Success for assassination is determined as for "
					"theft, i.e. the assassin will fail if any of the "
					"target faction's units can see him.  In this case, "
					"the assassin will flee, and the target faction will "
					"be informed which unit made the attempt.  As with "
					"theft, allies of the target unit will prevent the "
					"assassination from succeeding, if their Observation "
					"level is high enough.";
				f.Paragraph(temp);
				temp = "In addition, if the target is a mage, they have "
					"a chance to detect the assassination attempt. "
					"This chance depends on the mage's power; the more "
					"powerful the mage, the more likely they are to escape.";
				if(Globals->ARCADIA_MAGIC) f.Paragraph(temp);
				temp = "";
			} else {
				temp = "The assasin must know Stealth to attempt "
					"assassination.";
			}
			if(has_obse) {
				temp += "If the assassin has higher stealth than any of the "
					"target faction's units have Observation, then a "
					"one-on-one ";
			} else {
				temp += " A one-on-one ";
			}
			temp += "fight will take place between the assassin and the "
				"target character. The battle is handled like a normal "
				"fight, with the exception that the assassin cannot "
				"use any armour, and the victim may only use leather armour."; //BS mod, Arcadia only
			temp2 = "";
			last = -1;
/*			comma = 0;
			for(i = 0; i < NITEMS; i++) {
				if(!(ItemDefs[i].type & IT_ARMOR)) continue;
				if(!(ItemDefs[i].type & IT_NORMAL)) continue;
				if(ItemDefs[i].flags & ItemType::DISABLED) continue;
				ArmorType *at = FindArmor(ItemDefs[i].abr);
				if (at == NULL) continue;
				if (!(at->flags & ArmorType::USEINASSASSINATE) && !(at->flags & ArmorType::DEFINASSASSINATE)) continue;
				if(last == -1) {
					last = i;
					continue;
				}
				temp2 += ItemDefs[last].name;
				temp2 += ", ";
				last = i;
				comma++;
			}
			if(comma) temp2 += "or ";
			if(last != -1) {
				temp2 += ItemDefs[last].name;
				temp += " except ";
				temp += temp2;
			}
			temp += ".";*/
			if(last == -1)
				temp += " Armour ";
			else
				temp += " Most armour ";
			temp += "is forbidden for the assassin because it would "
				"make it too hard to sneak around, and for the victim "
				"because he was caught by surprise with his armour off. If "
				"the assassin wins, the target faction is told merely that "
				"the victim was assassinated, but not by whom.  If the "
				"victim wins, then the target faction learns which unit "
				"made the attempt.  (Of course, this does not necessarily "
				"mean that the assassin's faction is known.)  The winner of "
				"the fight gets 50% of the loser's property as usual.";
			f.Paragraph(temp);
			temp = f.Link("#steal", "STEAL") + " and " +
				f.Link("#assassinate", "ASSASSINATE") +
				" are not full month orders, and do not interfere with other "
				"activities, but a unit can only issue one " +
				f.Link("#steal", "STEAL") + " order or one " +
				f.Link("#assassinate", "ASSASSINATE") + " order in a month.";
			f.Paragraph(temp);
		}
	}
	f.LinkRef("magic");
	f.ClassTagText("div", "rule", "");
	f.TagText("h2", "Magic");
	if(Globals->ARCADIA_MAGIC) {
    	temp = "A character may enter the world of magic in Atlantis in one ways. Firstly, "
    		"a unit may study on one of the Foundation magic skills. Only one man units";
    	if(!Globals->MAGE_NONLEADERS && Globals->LEADERS_EXIST)
    		temp += ", with the man being a hero,";
    	temp += " are permitted to study these skills.";
    	
    	temp = "";
    	if(Globals->FACTION_LIMIT_TYPE != GameDefs::FACLIM_UNLIMITED) {
    		temp += "The number of these men (known as \"magicians\" or "
    			"\"mages\") that a faction may own is ";
    		if(Globals->FACTION_LIMIT_TYPE == GameDefs::FACLIM_MAGE_COUNT)
    			temp += "limited.";
    		else
    			temp += "determined by the faction's type.";
    		temp += " Any attempt to gain more, either through study, recruitment, or by "
    			"transfer from another faction, will fail.  In addition, mages ";
    	} else {
    		temp += "Mages ";
    	}
    	temp += "may not ";
    	temp += f.Link("#give", "GIVE") + " men at all; once a unit is a "
    		"mage, the unit number is "
    		"fixed, and men may not be added or removed. (The mage may be given to another faction using the ";
    	temp += f.Link("#give", "GIVE") + " UNIT order.)";
    	f.Paragraph(temp);
    	
	} else {    
    	temp = "A character enters the world of magic in Atlantis by beginning "
    		"study on one of the Foundation magic skills.  Only one man units";
    	if(!Globals->MAGE_NONLEADERS && Globals->LEADERS_EXIST)
    		temp += ", with the man being a leader,";
    	temp += " are permitted to study these skills. ";
    	if(Globals->FACTION_LIMIT_TYPE != GameDefs::FACLIM_UNLIMITED) {
    		temp += "The number of these units (known as \"magicians\" or "
    			"\"mages\") that a faction may own is ";
    		if(Globals->FACTION_LIMIT_TYPE == GameDefs::FACLIM_MAGE_COUNT)
    			temp += "limited.";
    		else
    			temp += "determined by the faction's type.";
    		temp += " Any attempt to gain more, either through study, or by "
    			"transfer from another faction, will fail.  In addition, mages ";
    	} else {
    		temp += "Mages ";
    	}
    	temp += "may not ";
    	temp += f.Link("#give", "GIVE") + " men at all; once a unit becomes a "
    		"mage (by studying one of the Foundations), the unit number is "
    		"fixed. (The mage may be given to another faction using the ";
    	temp += f.Link("#give", "GIVE") + " UNIT order.)";
    	f.Paragraph(temp);
	}
	f.LinkRef("magic_skills");
	f.TagText("h3", "Magic Skills:");
	temp = "Magic skills are the same as normal skills, with a few "
		"differences.  The basic magic skills, called Foundations, are: ";
	last = -1;
	comma = 0;
	j = 0;
	for(i = 0; i < NSKILLS; i++) {
		if(SkillDefs[i].flags & SkillType::DISABLED) continue;
		if(!(SkillDefs[i].flags & SkillType::FOUNDATION)) continue;
		j++;
		if(last == -1) {
			last = i;
			continue;
		}
		temp += SkillDefs[last].name;
		temp += ", ";
		comma++;
		last = i;
	}
	if(comma) temp += "and ";
	temp += SkillDefs[last].name;
	temp += ". ";
	if(Globals->ARCADIA_MAGIC) {
        temp += "Each of these skills is at the base of a 'field' of magic spells. "
            "To become a mage, a normal unit may undertake study in one of these "
    		"Foundations.  As a unit studies the Foundations, he will be able "
    		"to study deeper into the magical arts; the additional skills that "
    		"he may study will be indicated on your turn report. Foundation skills  "
    		"will not be included in this turn report list; they may be studied by any mage, "
            "or, if your faction can support an additional mage, by a non-mage unit.";
    	f.Paragraph(temp);
    	temp = "There are two major differences between Magic skills and most "
    		"normal skills. The first is that the ability to study Magic skills "
    		"sometimes depends on lower level Magic skills.  The Magic skills "
    		"that a mage may study are listed on his turn report, so he knows "
    		"which areas he may pursue.  Studying higher in the Foundation "
    		"skills, and certain other Magic skills, will make other skills "
    		"available to the mage. Experienced Atlantis players should note that, unlike "
            "in some Atlantis games, mages do not need to be located in a "
            "building in order to study spells to high level. ";
    	f.Paragraph(temp);
	} else {    
        temp += "To become a mage, a unit undertakes study in one of these "
    		"Foundations.  As a unit studies the Foundations, he will be able "
    		"to study deeper into the magical arts; the additional skills that "
    		"he may study will be indicated on your turn report.";
    	f.Paragraph(temp);
    	temp = "There are two major differences between Magic skills and most "
    		"normal skills. The first is that the ability to study Magic skills "
    		"sometimes depends on lower level Magic skills.  The Magic skills "
    		"that a mage may study are listed on his turn report, so he knows "
    		"which areas he may pursue.  Studying higher in the Foundation "
    		"skills, and certain other Magic skills, will make other skills "
    		"available to the mage. Also, study into a magic skill above "
    		"level 2 requires that the mage be located in some sort of "
    		"building which can ";
    	if(!Globals->LIMITED_MAGES_PER_BUILDING) {
    		temp += "offer protection.  Trade structures do not count. ";
    	} else {
    		temp += "offer specific protection to mages.  Certain types of "
    			"buildings can offer shelter and support and a proper "
    			"environment, some more so than others. ";
    	}
    	temp += "If the mage is not in such a structure, his study rate is cut "
    			"in half, as he does not have the proper environment and "
    			"equipment for research.";
    	f.Paragraph(temp);
    
    	if(Globals->LIMITED_MAGES_PER_BUILDING) {
    		temp = "It is possible that there are advanced buildings not listed "
    			"here which also can support mages.  The description of a "
    			"building will tell you for certain.  The common buildings and "
    			"the mages a building of that type can support follows:";
    		f.LinkRef("tablemagebuildings");
    		f.Enclose(1, "center");
    		f.Enclose(1, "table border=\"1\"");
    		f.Enclose(1, "tr");
    		f.TagText("td", "");
    		f.TagText("th", "Mages");
    		f.Enclose(0, "tr");
    		for(i = 0; i < NOBJECTS; i++) {
    			if(ObjectDefs[i].flags & ObjectType::DISABLED) continue;
    			if(!ObjectDefs[i].maxMages) continue;
    			pS = FindSkill(ObjectDefs[i].skill);
    			if(pS == NULL) continue;
    			if(pS->flags & SkillType::MAGIC) continue;
    			k = ObjectDefs[i].item;
    			if(k == -1) continue;
    			/* Need the >0 since item could be WOOD_OR_STONE (-2) */
    			if(k > 0 && (ItemDefs[k].flags & ItemType::DISABLED)) continue;
    			if(k > 0 && !(ItemDefs[k].type & IT_NORMAL)) continue;
    			/* Okay, this is a valid object to build! */
    			f.Enclose(1, "tr");
    			f.Enclose(1, "td align=\"left\" nowrap");
    			f.PutStr(ObjectDefs[i].name);
    			f.Enclose(0, "td");
    			f.Enclose(1, "td align=\"left\" nowrap");
    			f.PutStr(ObjectDefs[i].maxMages);
    			f.Enclose(0, "td");
    		}
    		f.Enclose(0, "table");
    		f.Enclose(0, "center");
    	}
	}
	
	f.LinkRef("magic_foundations");
	f.TagText("h3", "Foundations:");
	temp = "The ";
	temp += NumToWord(j);
	temp += " foundation skills are called ";
	last = -1;
	comma = 0;
	for(i = 0; i < NSKILLS; i++) {
		if(SkillDefs[i].flags & SkillType::DISABLED) continue;
		if(!(SkillDefs[i].flags & SkillType::FOUNDATION)) continue;
		if(last == -1) {
			last = i;
			continue;
		}
		temp += SkillDefs[last].name;
		temp += ", ";
		comma++;
		last = i;
	}
	if(comma) temp += "and ";
	temp += SkillDefs[last].name;
	temp += ".";
	/* XXX -- This needs better handling! */
	/* Add each foundation here if it exists */
	if(!(SkillDefs[S_FORCE].flags & SkillType::DISABLED)) {
		temp += " Force indicates the quantity of magical energy that a "
			"mage is able to channel (a Force rating of 0 does not mean "
			"that the mage can channel no magical energy at all, but only "
			"a minimal amount).";
	}
	if(!(SkillDefs[S_PATTERN].flags & SkillType::DISABLED)) {
		temp += " Pattern indicates ability to handle complex patterns, and "
			"is important for things like healing and nature spells. ";
	}
	if(!(SkillDefs[S_SPIRIT].flags & SkillType::DISABLED)) {
		temp += " Spirit deals with meta-effects that lie outside the scope "
			"of the physical world.";
	}
	if(!(SkillDefs[S_BASE_WINDKEY].flags & SkillType::DISABLED)) {
		temp += " Windkey is at the heart of control of the wind and weather. "
            "Mages skilled in windkey are especially sought after by ship captains, "
            "but many people may benefit from good winds and weather, while "
            "storms and lightning may be powerful offensive weapons. ";
	}
	if(!(SkillDefs[S_BASE_ILLUSION].flags & SkillType::DISABLED)) {
		temp += " Illusion indicates the ability to weave illusions in the "
            "air, making people see that which does not exist, or in some "
            "cases concealing that which does exist. It is not known why, "
            "but mages with the innate ability to weave illusions are most "
            "often found in the east. ";
	}
	if(!(SkillDefs[S_BASE_PATTERNING].flags & SkillType::DISABLED)) {
		temp += " Patterning indicates the ability of a mage to harness the "
            "energies of nature and the earth. Such abilities are usually used "
            "to nourish lands, but may be used for destruction as well. "
            "Mages with innate strength in patterning most commonly originate "
            "in the north. ";
	}
	if(!(SkillDefs[S_BASE_SUMMONING].flags & SkillType::DISABLED)) {
		temp += " Summoning deals with the ability of mages to reach in distant "
            "places or different worlds, and pluck out creatures to pull them "
            "here. Necromancy and the ability to communicate with the dead are "
            "the most famous abilities study in summoning may bestow. Natural ability "
            "in summoning is most common among the southern folk. ";
	}
	if(!(SkillDefs[S_BASE_MYSTICISM].flags & SkillType::DISABLED)) {
		temp += " Mysticism is the least understood, and arguably the most powerful "
            "of the magic fields. Study in mysticism allows a mage to harness "
            "energies from outside his body, as well as from within. This allows "
            "mages to cast spells more often, but is occasionally unpredictable. "
            "Mysticism mages are most famous not for the great works of magic they "
            "are capable of, but for the times when, just as it is most needed, "
            "their magic fizzles or even turns on its wielder. ";
	}
	f.Paragraph(temp);




	if(app_exist) {
		temp = "Apprentices may be created by having them study ";
		comma = 0;
		for(i = 0; i < NSKILLS; i++) {
			if(SkillDefs[i].flags & SkillType::DISABLED) continue;
			if(!(SkillDefs[i].flags & SkillType::APPRENTICE)) continue;
			if(last == -1) {
				last = i;
				continue;
			}
			temp += SkillDefs[last].name;
			temp += ", ";
			comma++;
			last = i;
		}
		if(comma) temp += "or ";
		temp += SkillDefs[last].name;
		temp += ". ";
		temp += "Apprentices may not cast spells, but may utilize items "
			"which otherwise only mages can use.";
		f.Paragraph(temp);
	}

	f.LinkRef("magic_furtherstudy");
	f.TagText("h3", "Further Magic Study:");
	if(Globals->ARCADIA_MAGIC) {
    	temp = "Once a mage has begun study of one or more Foundations, more "
    		"skills that he may study will begin to show up on his report. "
    		"These skills are the skills that allow a mage to cast spells.  As with "
    		"normal skills, when a mage achieves a new level of a magic skill, "
    		"he will be given a skill report, describing the new powers (if "
    		"any) that the new skill confers.  The ";
    	temp += f.Link("#show", "SHOW") + " order may be used to show this "
    		"information on future reports.";
    	f.Paragraph(temp);
    	temp = "A mage can also gain skill levels in magic through experience; "
    	    "this may be gained in a number of ways. Most commonly, when spells are ";
	    temp += f.Link("#cast", "CAST") + " or used in combat, the mage will "
	        "gain experience in that spell. This experience will be halved if "
	        "the mage does not specialise in the spell's field of magic. In "
	        "addition, when a mage travels the world, he will discover more "
	        "about the nature of magic, and gain opportunities to practise "
	        "his magic apart from his other activities. Whenever a mage "
	        "uses a ";
        temp += f.Link("#move", "MOVE") + " or " + f.Link("#advance", "ADVANCE") +
            "order, he has a chance to gain experience in a random skill. On "
            "average, a travelling mage will gain experience at the same rate "
            "as a mage who continues studying a skill after gaining his maximum knowledge "
            "level in that skill.";
    	f.Paragraph(temp);
	} else {
    	temp = "Once a mage has begun study of one or more Foundations, more "
    		"skills that he may study will begin to show up on his report. "
    		"These skills are the skills that give a mage his power.  As with "
    		"normal skills, when a mage achieves a new level of a magic skill, "
    		"he will be given a skill report, describing the new powers (if "
    		"any) that the new skill confers.  The ";
    	temp += f.Link("#show", "SHOW") + " order may be used to show this "
    		"information on future reports.";
    	f.Paragraph(temp);
	}
	f.LinkRef("magic_usingmagic");
	f.TagText("h3", "Using Magic:");
	if(Globals->ARCADIA_MAGIC) {
    	temp = "A mage may use his magical power in three different ways, "
    		"depending on the type of spell he wants to use.  Some spells, "
    		"once learned, take effect automatically and are considered "
    		"always to be in use; these spells do not require any order to "
    		"take effect.";
    	f.Paragraph(temp);
    	temp = "Secondly, some spells are for use in combat. A mage may specify "
    		"that he wishes to use a spell in combat by issuing the ";
    	temp += f.Link("#combat", "COMBAT") + " order.  A combat spell "
    		"specified in this way will only be used if the mage finds "
    		"himself taking part in a battle.";
    	f.Paragraph(temp);
    	temp = "The third type of spell use is for spells that take an entire "
    		"month to cast.  These spells are cast by the mage issuing the ";
    	temp += f.Link("#cast", "CAST") + " order. A mage may also "
            "issue up to three CAST orders each month, providing "
            "that each cast order is for a different spell, and that no more than "
            "one spell moves the mage (through teleportation or gate "
            "jumping). ";
    	f.Paragraph(temp);
    	temp = "Every spell that is cast, or used in combat, will drain the mage of "
            "energy. How much energy is needed for a spell will be specified in the "
            "skill description that a mage recieved when he first learns a new "
            "spell. All energy costs are higher if the mage's speciality is in "
            "a different field of magic (or if the mage has no speciality), this "
            "will also be specified in the skill description. If multiple spells are ";
        temp += f.Link("#cast", "CAST") + ", then the second spell cast will cost "
            "50% more energy than usual, and the third spell cast will cost twice "
            "as much energy as usual. For this reason, it is usually advisable to "
            "cast the most energy-intensive spell first, and the cheapest one last, "
            "so that the extra energy cost is minimised.";
    	f.Paragraph(temp);
	} else {
    	temp = "A mage may use his magical power in three different ways, "
    		"depending on the type of spell he wants to use.  Some spells, "
    		"once learned, take effect automatically and are considered "
    		"always to be in use; these spells do not require any order to "
    		"take effect.";
    	f.Paragraph(temp);
    	temp = "Secondly, some spells are for use in combat. A mage may specify "
    		"that he wishes to use a spell in combat by issuing the ";
    	temp += f.Link("#combat", "COMBAT") + " order.  A combat spell "
    		"specified in this way will only be used if the mage finds "
    		"himself taking part in a battle.";
    	f.Paragraph(temp);
    	temp = "The third type of spell use is for spells that take an entire "
    		"month to cast.  These spells are cast by the mage issuing the ";
    	temp += f.Link("#cast", "CAST") + " order. Because " +
    		f.Link("#cast", "CAST") + " takes an entire month, a mage may use "
    		"only one of this type of spell each turn. Note, however, that a ";
    	temp += f.Link("#cast", "CAST") + " order is not a full month order; "
    		"a mage may still ";
    	temp += f.Link("#move", "MOVE") + ", ";
    	temp += f.Link("#study", "STUDY") + ", or use any other month long order. ";
    	temp += "The justification for this (as well as being for game balance) "
    		"is that a spell drains a mage of his magic power for the month, "
    		"but does not actually take the entire month to cast.";
    	f.Paragraph(temp);
    	temp = "The description that a mage receives when he first learns a "
    		"spell specifies the manner in which the spell is used (automatic, "
    		"in combat, or by casting).";
    	f.Paragraph(temp);
	}
	if(Globals->ARCADIA_MAGIC) {
    	f.LinkRef("magic_energy");
    	f.TagText("h3", "Magical Energy:");
    	temp = "Every mage has a limited supply of energy, which is partially regenerated every turn. "
            "This energy supply depends on the mage's level in the fundamental skills; other magic "
            "skills allow a mage to cast new spells, but do not increase the mage's energy supply. "
            "For each fundamental magic skill that a mage is skilled in, his maximum energy storage "
            "will be increased by his level in that skill, squared, while the rate at which he regains "
            "energy will be increased by his skill level. If a mage is specialised in that fundamental, "
            "then his increased affinity to that field of magic doubles the contribution of that skill. "
            "The only exception to this is the mysticism fundamental, which increases a mage's energy "
            "at a greater rate. An exact description of how mysticism works may be obtained by reading "
            "the mysticism skill description.";
    	f.Paragraph(temp);
    	temp = "If a mage runs out of energy, then he will be unable to ";
    	temp += f.Link("#cast", "CAST") + " any more spells. The exception is in combat, when a mage will be driven "
            "to cast spells even if he has no energy; however these spells will be cast at only "
            "half their usual skill level, rounded down (if a mage has his combat spell only at "
            "level 1, then without energy he will not cast at all). Some magical skills also create "
            "items which cause an ongoing energy drain for the mage to sustain them. These costs reduce "
            "a mage's rate of energy recharge, and if the mage cannot regenerate enough energy to "
            "sustain them, the items will be lost.";
    	f.Paragraph(temp);
	}
	f.LinkRef("magic_incombat");
	f.TagText("h3", "Magic in Combat:");
	temp = "NOTE: This section is rather vague, and quite advanced.  You "
		"may want to wait until you have figured out other parts of "
		"Atlantis before trying to understand exactly all of the rules in "
		"this section.";
	f.Paragraph(temp);
	temp = "Although the magic skills and spells are unspecified in these "
		"rules, left for the players to discover, the rules for combat "
		"spells' interaction are spelled out here.  There are five major "
		"types of attacks, and defenses: Combat, Ranged, Energy, Weather, "
		"and Spirit.  Every attack and defense has a type, and only the "
		"appropriate defense is effective against an attack.";
	f.Paragraph(temp);
	temp = "Defensive spells are cast at the beginning of each round of "
		"combat, and will have a type of attack they deflect, and skill "
		"level (Defensive spells are generally called Shields).  Every "
		"time an attack is launched against an army, it must first attack "
		"the highest level Shield of the same type as the attack, before "
		"it may attack a soldier directly. Note that an attack only has "
		"to attack the highest Shield, any other Shields of the same "
		"type are ignored for that attack.";
	f.Paragraph(temp);
	temp = "An attack spell (and any other type of attack) also has an "
		"attack type, and attack level, and a number of blows it deals. "
		"When the attack spell is cast, it is matched up against the most "
		"powerful defensive spell of the appropriate type that the other "
		"army has cast. If the other army has not cast any applicable "
		"defensive spells, the attack goes through unmolested. For "
        "every blow the spell attempts to deal, there is usually a "
        "50% chance of getting the opportunity for a lethal hit, "
        "although as with other attacks, this chance may be modified "
        "by position, terrain, fortifications or other spells. As with "
		"normal combat, men which are in the open (not protected by "
		"a building) have an effective skill against magic attacks of 0, unless they have a "
		"shield or some other defensive magic. Some monsters "
		"have bonuses to resisting some attacks but are more susceptible "
		"to others. The skill level of the attack spell and the effective "
		"skill for defense are matched against each other.  The formula "
		"for determining the victor between a defensive shield and offensive "
		"spell is the same as for a contest of soldiers; if the levels "
		"are equal, there is a 1:1 chance of success, and so on. If the "
		"offensive spell is victorious, the offensive spell deals its blows "
		"to the defending army, and the Shield in question loses one level in power "
		"(thus, a level 5 shield will be destroyed after five spells have "
		"passed through "
		"it). Otherwise, the attack spell disperses, and "
		"the defending spell remains in place.";
	f.Paragraph(temp);
	temp = "Some spells do not actually kill enemies, but rather have some "
		"negative effect on them. These spells are treated the same as "
		"normal spells; if there is a Shield of the same type as them, "
		"they must attack the Shield before attacking the army. However, "
        "these spells will not cause a shield to be weakened. "
		"Physical attacks that go through a defensive spell also must "
		"match their skill level against that of the defensive spell in "
		"question.  However, if they pass through the shield, they only "
        "have a 1% chance of weakening the shield. Note that this check "
        "is done before the check for chance-to-attack for the bowmen, "
        "so this is effectively a 2% chance per attack.";
	f.Paragraph(temp);
	if(Globals->EARTHSEA_VICTORY) {
    	f.LinkRef("magic_mastery");
    	f.TagText("h3", "Magic Mastery:");
    	temp = "Before the destruction of Bashkeil, the school at Alanum "
    	    "was the primary location for all teaching of magic. It was "
    	    "here that the six Masters were appointed; one to head each school "
    	    "of magic. The role of a Master is more than a title; when a "
    	    "candidate was appointed, they would spend a month in meditation "
    	    "of the path they should take. It is said that if one who was "
    	    "unworthy was appointed, he would never wake to take the mantle "
    	    "of master.";
	    temp = " The role of a Master is not a light one. They must always make "
	        "themselves available to students of the arts, and to help those "
	        "non-magicians who need their aid. As such, they may never walk in "
	        "secret, and may not travel further than 15 regions from Alanum for "
            "an extended period - a distance which would take them beyond "
            "the major islands of the world. Likewise, there is a limit to how "
            "long a Master may spend at sea or underground before he loses "
            "the mantle of a Master. The benefits are small; a Master will gain "
            "a one-tenth bonus to the rate at which his energy recharges. But "
            "there are some abilities which only Masters may hold, and only "
            "those mages who are known as Masters will be able to face, and fix, "
            "the problems around Bashkeil.";
	    f.Paragraph(temp);
        temp = " There have been occasions, during times of conflict "
    	    "or confusion, when two mages been appointed Master "
    	    "and stood in challenge for the title. In our times of trouble, "
    	    "with six Masters dead and none left to appoint new Masters "
            "it is inevitable that mages may claim the title unto themselves. "
            "However, the magic of the world itself prevents more than two "
            "mages from assuming the role of Master; more may try, but are "
            "bound to fail. In such a circumstance, only if a Master "
            "relinquishes his title, or is killed, may another mage take "
            "his place.";
	    f.Paragraph(temp);
	    temp = " For each of the fields of magic, the requirements for a mage "
	        "to be able to become a Master are different. Generally, a mage "
	        "must know a particular spell to level 5. Knowledge of which spell is necessary may be "
	        "provided to you with the skilltree mages may study, or may perhaps be found "
	        "only within the game world. To begin the meditation required to "
            "assume the title of Master, a mage should issue a ";
        temp += f.Link("#master", "MASTER") + " order.";
	    f.Paragraph(temp);
	}
	f.LinkRef("nonplayers");
	f.ClassTagText("div", "rule", "");
	f.TagText("h2", "Non-Player Units");
	temp = "There are a number of units that are not controlled by players "
		"that may be encountered in Atlantis.  Most information about "
		"these units must be discovered in the course of the game, but a "
		"few basics are below.";
	f.Paragraph(temp);
	if(Globals->TOWNS_EXIST && Globals->CITY_MONSTERS_EXIST) {
		f.LinkRef("nonplayers_guards");
		f.TagText("h3", "City and Town Guardsmen:");
		temp = "All cities and towns begin with guardsmen in them.  These "
			"units will defend any units that are attacked in the city or "
			"town, and will also prevent theft and assassination attempts, ";
		if(has_obse)
			temp += "if their Observation level is high enough. ";
		else
			temp += "if they can see the criminal. ";
		temp += "They are on guard, and will prevent other units from "
			"taxing or pillaging. ";
		if(Globals->SAFE_START_CITIES)
			temp += "Except in the starting cities, the ";
		else
			temp += "The ";
		temp += "guards may be killed by players, although they will form "
			"again if the city is left unguarded.";
		f.Paragraph(temp);
		if(Globals->SAFE_START_CITIES || Globals->START_CITY_GUARDS_PLATE ||
				Globals->START_CITY_MAGES) {
			if(Globals->AMT_START_CITY_GUARDS) {
				temp = "Note that the city guardsmen in the starting cities "
					"of Atlantis possess ";
				if(Globals->SAFE_START_CITIES)
					temp += "Amulets of Invincibility ";
				if(Globals->START_CITY_GUARDS_PLATE) {
					if(Globals->SAFE_START_CITIES) temp += "and ";
					temp += "plate armour ";
				}
				temp += "in addition to being more numerous and ";
				if(Globals->SAFE_START_CITIES)
					temp += "may not be defeated.";
				else
					temp += "are therefore harder to kill.";
			}

			if(Globals->START_CITY_MAGES) {
				if(Globals->AMT_START_CITY_GUARDS)
					temp += " Additionally, in ";
				else
					temp += "In ";
				temp += "the starting cities, Mage Guards will be found. "
					"These mages are adept at the fire spell";
				if(!Globals->SAFE_START_CITIES) {
					temp += " making any attempt to control a starting "
						"city a much harder proposition";
				}
				temp += ".";
			}
			f.Paragraph(temp);
		}
	}
	if (Globals->WANDERING_MONSTERS_EXIST) {
		f.LinkRef("nonplayers_monsters");
		f.TagText("h3", "Wandering Monsters:");
		temp = "There are a number of monsters who wander free throughout "
			"Atlantis.  They will occasionally attack player units, so be "
			"careful when wandering through the wilderness.";
		f.Paragraph(temp);
	}
	f.LinkRef("nonplayers_controlled");
	f.TagText("h3", "Controlled Monsters:");
	temp = "Through various magical methods, you may gain control of "
		"certain types of monsters. These monsters are just another item "
		"in a unit's inventory, with a few special rules. Monsters will "
		"be able to carry things at their speed of movement; use the ";
	temp += f.Link("#show", "SHOW") + " ITEM order to determine the "
		"carrying capacity and movement speed of a monster. Monsters will "
		"also fight for the controlling unit in combat; their strength "
		"can only be determined in battle. Also, note that a monster will "
		"always fight from the front rank, even if the controlling unit "
		"has the behind flag set. Whether or not you are allowed to give a "
		"monster to other units depends on the type of monster; some may be "
		"given freely, while others must remain with the controlling unit.";
	if(Globals->RELEASE_MONSTERS) {
		temp += " All monsters may be released completely by using the ";
		temp += f.Link("#give", "GIVE") + " order targetting unit 0.  When "
			"this is done, the monster will become a wandering monster.";
	}
	f.Paragraph(temp);
	f.LinkRef("orders");
	f.ClassTagText("div", "rule", "");
	f.TagText("h2", "Orders");
	temp = "To enter orders for Atlantis, you should send a mail message "
		"to the Atlantis server, containing the following:";
	f.Paragraph(temp);
	f.Paragraph("");
	f.Enclose(1, "pre");
	f.ClearWrapTab();
	f.WrapStr("#ATLANTIS faction-no <password>");
	f.PutNoFormat("");
	f.WrapStr("UNIT unit-no");
	f.WrapStr("...orders...");
	f.PutNoFormat("");
	f.WrapStr("UNIT unit-no");
	f.WrapStr("...orders...");
	f.PutNoFormat("");
	f.WrapStr("#END");
	f.Enclose(0, "pre");
	temp = "For example, if your faction number (shown at the top of your "
		"report) is 27, your password if \"foobar\", and you have two "
		"units numbered 5 and 17:";
	f.Paragraph(temp);
	f.Paragraph("");
	f.Enclose(1, "pre");
	f.WrapStr("#ATLANTIS 27 \"foobar\"");
	f.PutNoFormat("");
	f.WrapStr("UNIT 5");
	f.WrapStr("...orders...");
	f.PutNoFormat("");
	f.WrapStr("UNIT 17");
	f.WrapStr("...orders...");
	f.PutNoFormat("");
	f.WrapStr("#END");
	f.Enclose(0, "pre");
	temp = "Thus, orders for each unit are given separately, and indicated "
		"with the UNIT keyword.  (In the case of an order, such as the "
		"command to rename your faction, that is not really for any "
		"particular unit, it does not matter which unit issues the command; "
		"but some particular unit must still issue it.)";
	f.Paragraph(temp);
	temp = "IMPORTANT: You MUST use the correct #ATLANTIS line or else your "
		"orders will be ignored.";
	f.Paragraph(temp);
	temp = "If you have a password set, you must specify it on you "
		"#atlantis line, or the game will reject your orders.  See the ";
	temp += f.Link("#password", "PASSWORD") + " order for more details.";
	f.Paragraph(temp);
	temp = "Each type of order is designated by giving a keyword as the "
		"first non-blank item on a line.  Parameters are given after this, "
		"separated by spaces or tabs. Blank lines are permitted, as are "
		"comments; anything after a semicolon is treated as a comment "
		"(provided the semicolon is not in the middle of a word).";
	f.Paragraph(temp);
	temp = "The parser is not case sensitive, so all commands may be given "
		"in upper case, lower case or a mixture of the two.  However, when "
		"supplying names containing spaces, the name must be surrounded "
		"by double quotes, or else underscore characters must be used in "
		"place of spaces in the name.  (These things apply to the #ATLANTIS "
		"and #END lines as well as to order lines.)";
	f.Paragraph(temp);
	temp = "You may precede orders with the at sign (@), in which case they "
		"will appear in the Template at the bottom of your report.  This is "
		"useful for orders which your units repeat for several months in a "
		"row. For example, to tax every turn:";
	f.Paragraph(temp);
	temp = "@TAX";
	f.Paragraph(temp);
	
	f.LinkRef("orders_abbreviations");
	f.TagText("h3", "Abbreviations:");
	temp = "All common items and skills have abbreviations that can be used "
		"when giving orders, for brevity.  Any time you see the item on your "
		"report, it will be followed by the abbreviation.  Please be careful "
		"using these, as they can easily be confused.";
	f.Paragraph(temp);
	f.LinkRef("ordersummary");
	f.ClassTagText("div", "rule", "");
	f.TagText("h2", "Order Summary");
	temp = "To specify a [unit], use the unit number.  If specifying a "
		"unit that will be created this turn, use the form \"NEW #\" if "
		"the unit belongs to your faction, or \"FACTION # NEW #\" if the "
		"unit belongs to a different faction.  See the ";
	temp += f.Link("#form", "FORM");
	temp += " order for a more complete description.  [faction] means that "
		"a faction number is required; [object] means that an object "
		"number (generally the number of a building or ship) is required. "
		"[item] means an item (like wood or longbow) that a unit can have "
		"in its possession. [flag] is an argument taken by several orders, "
		"that sets or unsets a flag for a unit. A [flag] value must be "
		"either 1 (set the flag) or 0 (unset the flag).  Other parameters "
		"are generally numbers or names.";
	f.Paragraph(temp);
	temp = "IMPORTANT: Remember that names containing spaces (e.g., "
		"\"Plate Armour\"), must be surrounded by double quotes, or the "
		"spaces must be replaced with underscores \"_\" (e.g., Plate_Armour).";
	f.Paragraph(temp);
	temp = "Also remember that anything used in an example is just that, "
		"an example and makes no gaurentee that such an item, structure, "
		"or skill actually exists within the game.";
	f.Paragraph(temp);

	f.ClassTagText("div", "rule", "");
	f.LinkRef("address");
	f.TagText("h4", "ADDRESS [new address]");
	f.Paragraph("Change the email address to which your reports are sent.");
	f.Paragraph("Example:");
	temp = "Change your faction's email address to atlantis@rahul.net.";
	temp2 = "ADDRESS atlantis@rahul.net";
	f.CommandExample(temp, temp2);

	f.ClassTagText("div", "rule", "");
	f.LinkRef("advance");
	f.TagText("h4", "ADVANCE [dir] ...");
	temp = "This is the same as the ";
	temp += f.Link("#move", "MOVE");
	temp += " order, except that it implies attacks on units which attempt "
		"to forbid access.  See the ";
	temp += f.Link("#move", "MOVE") + " order for details.";
	f.Paragraph(temp);
	f.Paragraph("Examples:");
	temp = "Move north, then northwest, attacking any units that forbid "
		"access to the regions.";
	temp2 = "ADVANCE N NW";
	f.CommandExample(temp, temp2);
	temp = "In order, move north, then enter structure number 1, move "
		"through an inner route, and finally move southeast. Will attack "
		"any units that forbid access to any of these locations.";
	temp2 = "ADVANCE N 1 IN SE";
	f.CommandExample(temp, temp2);

	if(Globals->USE_WEAPON_ARMOR_COMMAND) {
		f.ClassTagText("div", "rule", "");
		f.LinkRef("armour");
		f.TagText("h4", "ARMOUR [item1] [item2] [item3] [item4]");
		f.TagText("h4", "ARMOUR");
		temp = "This command allows you to set a list of preferred armour "
			"for a unit.  After searching for armour on the preferred "
			"list, the standard armour precedence takes effect if an armour "
			"hasn't been set.  The second form clears the preferred armour "
			"list.";
		f.Paragraph(temp);
		f.Paragraph("Examples");
		temp = "Set the unit to select chain armour before plate armour.";
		temp2 = "ARMOUR CARM PARM";
		f.CommandExample(temp, temp2);
		temp = "Clear the preferred armor list.";
		temp2 = "ARMOUR";
		f.CommandExample(temp, temp2);
	}

	if(has_stea) {
		f.ClassTagText("div", "rule", "");
		f.LinkRef("assassinate");
		f.TagText("h4", "ASSASSINATE [unit]");
		temp = "Attempt to assassinate the specified unit, or one of the "
			"unit's people if the unit contains more than one person.  The "
			"order may only be issued by a one-man unit.";
		f.Paragraph(temp);
		temp = "A unit may only attempt to assassinate a unit which is able "
			"to be seen.";
		f.Paragraph(temp);
		f.Paragraph("Example:");
		temp = "Assassinate unit number 177.";
		temp2 = "ASSASSINATE 177";
		f.CommandExample(temp, temp2);
	}

	f.ClassTagText("div", "rule", "");
	f.LinkRef("attack");
	f.TagText("h4", "ATTACK [unit] ... ");
	temp = "Attack a target unit.  If multiple ATTACK orders are given, "
		"all of the targets will be attacked.";
	f.Paragraph(temp);
	f.Paragraph("Example:");
	temp = "To attack units 17, 431, and 985:";
	temp2 = "ATTACK 17\nATTACK 431 985";
	f.CommandExample(temp, temp2);
	temp = "or:";
	temp2 = "ATTACK 17 431 985";
	f.CommandExample(temp, temp2);

	f.ClassTagText("div", "rule", "");
	f.LinkRef("autotax");
	f.TagText("h4", "AUTOTAX [flag]");
	temp = "AUTOTAX 1 causes the unit to attempt to tax every turn "
		"(without requiring the TAX order) until the flag is unset. "
		"AUTOTAX 0 unsets the flag.";
	f.Paragraph(temp);
	f.Paragraph("Example:");
	temp = "To cause the unit to attempt to tax every turn.";
	temp2 = "AUTOTAX 1";
	f.CommandExample(temp, temp2);

	f.ClassTagText("div", "rule", "");
	f.LinkRef("avoid");
	f.TagText("h4", "AVOID [flag]");
	temp = "AVOID 1 instructs the unit to avoid combat wherever possible. "
		"The unit will not enter combat unless it issues an ATTACK order, "
		"or the unit's faction is attacked in the unit's hex. AVOID 0 "
		"cancels this.";
	f.Paragraph(temp);
	temp = "The Guard and Avoid Combat flags are mutually exclusive; "
		"setting one automatically cancels the other.";
	f.Paragraph(temp);
	f.Paragraph("Example:");
	temp = "Set the unit to avoid combat when possible.";
	temp2 = "AVOID 1";
	f.CommandExample(temp, temp2);

	if(Globals->ALLOW_BANK & GameDefs::BANK_ENABLED) {
		f.ClassTagText("DIV", "rule", "");
		f.LinkRef("bank");
		f.TagText("H4", "BANK DEPOSIT amount");
		f.TagText("H4", "BANK WITHDRAW amount");
		temp = "The BANK order is used to deposit or withdraw silver from the bank.";
		f.Paragraph(temp);
		f.Paragraph("Example:");
		temp = "Deposit 2500 silver in the bank.";
		temp2 = "BANK DEPOSIT 2500";
		f.CommandExample(temp, temp2);
	}
	f.ClassTagText("div", "rule", "");
	f.LinkRef("behind");
	f.TagText("h4", "BEHIND [flag]");
	temp = "BEHIND 1 sets the unit to be behind other units in combat.  "
		"BEHIND 0 cancels this.";
	f.Paragraph(temp);
	f.Paragraph("Example:");
	temp = "Set the unit to be in front in combat.";
	temp2 = "BEHIND 0";
	f.CommandExample(temp, temp2);

	f.ClassTagText("div", "rule", "");
	f.LinkRef("build");
	f.TagText("h4", "BUILD");
	f.TagText("h4", "BUILD [object type]");
	f.TagText("h4", "BUILD [object type] [direction]");
	f.TagText("h4", "BUILD HELP [unit]");
	if(Globals->HEXSIDE_TERRAIN) {
    	temp = "BUILD given with no parameters causes the unit to perform work "
    		"on the object that it is currently inside.  BUILD given with an "
    		"[object type] (such as \"Tower\" or \"Galleon\") instructs the unit "
    		"to begin work on a new object of the type given. If this object is "
            "a seagoing vessel (anything that cannot be present in the centre of "
            "a land hex) then the third form must be used, specifying which edge "
            "of the region the object is to be built in. The final form "
    		"instructs the unit to enter the same building as [unit] and to "
    		"assist in building that structure, even if it is a structure which "
    		"was begun that same turn. This help will be rejected if the unit "
    		"you are helping does not consider you to be friendly. Building of "
            "features which only exist on the edge of regions (such as bridges "
            "and roads) should always use the third form. For these objects, "
            "the third form may be used by multiple units, and all will perform "
            "work on the same feature.";
    	f.Paragraph(temp);
    	f.Paragraph("Examples:");
    	temp = "To build a new tower.";
    	temp2 = "BUILD Tower";
    	f.CommandExample(temp, temp2);
    	temp = "To continue building a partially built road to the north-east.";
    	temp2 = "BUILD Road NE";
    	f.CommandExample(temp, temp2);
    	temp = "To help unit 5789 build a structure.";
    	temp2 = "BUILD HELP 5789";
    	f.CommandExample(temp, temp2);
	} else {
    	temp = "BUILD given with no parameters causes the unit to perform work "
    		"on the object that it is currently inside.  BUILD given with an "
    		"[object type] (such as \"Tower\" or \"Galleon\") instructs the unit "
    		"to begin work on a new object of the type given. The final form "
    		"instructs the unit to enter the same building as [unit] and to "
    		"assist in building that structure, even if it is a structure which "
    		"was begun that same turn.  This help will be rejected if the unit "
    		"you are helping does not consider you to be friendly.";
    	f.Paragraph(temp);
    	f.Paragraph("Examples:");
    	temp = "To build a new tower.";
    	temp2 = "BUILD Tower";
    	f.CommandExample(temp, temp2);
    	temp = "To help unit 5789 build a structure.";
    	temp2 = "BUILD HELP 5789";
    	f.CommandExample(temp, temp2);
	}

	f.ClassTagText("div", "rule", "");
	f.LinkRef("buy");
	f.TagText("h4", "BUY [quantity] [item]");
	f.TagText("h4", "BUY ALL [item]");
	temp = "Attempt to buy a number of the given item from a city or town "
		"marketplace, or to buy new people in any region where people are "
		"available for recruiting.  If the unit can't afford as many as "
		"[quantity], it will attempt to buy as many as it can. If the "
		"demand for the item (from all units in the region) is greater "
		"than the number available, the available items will be split "
		"among the buyers in proportion to the amount each buyer attempted "
		"to buy. ";
	if (Globals->RACES_EXIST) {
		temp += "When buying people, specify the race of the people as the "
			"[item]. ";
	}
	temp += "If the second form is specified, the unit will attempt to buy "
		"as many as it can afford.";
	f.Paragraph(temp);
	f.Paragraph(AString("Example") + (Globals->RACES_EXIST?"s":"") + ":");
	temp = "Buy one plate armour from the city market.";
	temp2 = "BUY 1 \"Plate Armour\"";
	f.CommandExample(temp, temp2);
	if (Globals->RACES_EXIST) {
		temp = "Recruit 5 barbarians into the current unit. (This will "
			"dilute the skills that the unit has.)";
		temp2 = "BUY 5 barbarians";
		f.CommandExample(temp, temp2);
	}

	f.ClassTagText("div", "rule", "");
	f.LinkRef("cast");
	f.TagText("h4", "CAST [skill] [arguments]");
	temp = "Cast the given spell.  Note that most spell names contain "
		"spaces; be sure to enclose the name in quotes!  [arguments] "
		"depends on which spell you are casting; when you are able to cast "
		"a spell, the skill description will tell you the syntax.";
	f.Paragraph(temp);
	f.Paragraph("Examples:");
	temp = "Cast the spell called \"Super Spell\".";
	temp2 = "CAST \"Super Spell\"";
	f.CommandExample(temp, temp2);
	temp = "Cast the fourth-level spell in the \"Super Magic\" skill.";
	temp2 = "CAST Super_Magic 4";
	f.CommandExample(temp, temp2);

	f.ClassTagText("div", "rule", "");
	f.LinkRef("claim");
	f.TagText("h4", "CLAIM [amount]");
	temp = "Claim an amount of the faction's unclaimed silver, and give it "
		"to the unit issuing the order.  The claiming unit may then spend "
		"the silver or give it to another unit.";
	f.Paragraph(temp);
	f.Paragraph("Example:");
	temp = "Claim 100 silver.";
	temp2 = "CLAIM 100";
	f.CommandExample(temp, temp2);

	f.ClassTagText("div", "rule", "");
	f.LinkRef("combat");
	f.TagText("h4", "COMBAT [spell]");
	f.TagText("h4", "COMBAT");
	temp = "Set the given spell as the spell that the unit will cast in "
		"combat.  This order may only be given if the unit can cast the "
		"spell in question. The second form of the order may be used "
        "to clear the mage's combat spell, if you do not wish him to "
        "cast a spell in combat.";
	f.Paragraph(temp);
	f.Paragraph("Example:");
	temp = "Instruct the unit to use the spell \"Super Spell\", when the "
		"unit is involved in a battle.";
	temp2 = "COMBAT \"Super Spell\"";
	f.CommandExample(temp, temp2);

	if (Globals->FOOD_ITEMS_EXIST) {
		f.ClassTagText("div", "rule", "");
		f.LinkRef("consume");
		f.TagText("h4", "CONSUME UNIT");
		f.TagText("h4", "CONSUME FACTION");
		f.TagText("h4", "CONSUME");
		temp = "The CONSUME order instructs the unit to use food items in "
			"preference to silver for maintenance costs. CONSUME UNIT tells "
			"the unit to use food items that are in that unit's possession "
			"before using silver. CONSUME FACTION tells the unit to use any "
			"food items that the faction owns (in the same region as the "
			"unit) before using silver. CONSUME tells the unit to use "
			"silver before food items (this is the default).";
		f.Paragraph(temp);
		f.Paragraph("Example:");
		temp = "Tell a unit to use food items in the unit's possession for "
			"maintenance costs.";
		temp2 = "CONSUME UNIT";
		f.CommandExample(temp, temp2);
	}

	f.ClassTagText("div", "rule", "");
	f.LinkRef("declare");
	f.TagText("h4", "DECLARE [faction] [attitude]");
	f.TagText("h4", "DECLARE [faction]");
	f.TagText("h4", "DECLARE DEFAULT [attitude]");
	temp = "The first form of the DECLARE order sets the attitude of your "
		"faction towards the given faction.  The second form cancels any "
		"attitude towards the given faction (so your faction's attitude "
		"towards that faction will be its default attitude).  The third "
		"form sets your faction's default attitude.";
	f.Paragraph(temp);
	f.Paragraph("Examples:");
	temp = "Declare your faction to be hostile to faction 15.";
	temp2 = "DECLARE 15 hostile";
	f.CommandExample(temp, temp2);
	temp = "Set your faction's attitude to faction 15 to its default "
		"attitude.";
	temp2 = "DECLARE 15";
	f.CommandExample(temp, temp2);
	temp = "Set your faction's default attitude to friendly.";
	temp2 = "DECLARE DEFAULT friendly";
	f.CommandExample(temp, temp2);

	f.ClassTagText("div", "rule", "");
	f.LinkRef("describe");
	f.TagText("h4", "DESCRIBE UNIT [new description]");
	f.TagText("h4", "DESCRIBE SHIP [new description]");
	f.TagText("h4", "DESCRIBE BUILDING [new description]");
	f.TagText("h4", "DESCRIBE OBJECT [new description]");
	f.TagText("h4", "DESCRIBE STRUCTURE [new description]");
	temp = "Change the description of the unit, or of the object the unit "
		"is in (of which the unit must be the owner). Descriptions can be "
		"of any length, up to the line length your mailer can handle. If "
		"no description is given, the description will be cleared out. The "
		"last four are completely identical and serve to modify the "
		"description of the object you are currently in.";
	f.Paragraph(temp);
	f.Paragraph("Example:");
	temp = "Set the unit,s description to read \"Merlin's helper\".";
	temp2 = "DESCRIBE UNIT \"Merlin's helper\"";
	f.CommandExample(temp, temp2);

	f.ClassTagText("div", "rule", "");
	f.LinkRef("destroy");
	f.TagText("h4", "DESTROY");
	temp = "Destroy the object you are in (of which you must be the owner). "
		"The order cannot be used at sea.";
	f.Paragraph(temp);
	f.Paragraph("Example:");
	temp = "Destroy the current object";
	temp2 = "DESTROY";
	f.CommandExample(temp, temp2);

	if (qm_exist) {
		f.ClassTagText("div", "rule", "");
		f.LinkRef("distribute");
		f.TagText("h4", "DISTRIBUTE [unit] [num] [item]");
		f.TagText("h4", "DISTRIBUTE [unit] ALL [item]");
		f.TagText("h4", "DISTRIBUTE [unit] ALL [item] EXCEPT [amount]");
		temp = "Distribute the specified items to the given friendly unit. "
			"In the second form, all of that item, are distributed.  In the "
			"last form, all of that item except for the specified amount "
			"are distributed.";
		temp += " The unit issuing the distribute order must have the "
			"quartermaster skill, and be the owner of a transport "
			"structure. Use of this order counts as trade activity in "
			"the hex.";
		f.Paragraph(temp);
		f.Paragraph("Examples:");
		temp = "Distribute 10 STON to unit 1234";
		temp2 = "DISTRIBUTE 1234 10 STON";
		f.CommandExample(temp, temp2);
		temp = "Distribute all except 10 SWOR to unit 3432";
		temp2 = "DISTRIBUTE 3432 ALL SWOR EXCEPT 10";
	}

	f.ClassTagText("div", "rule", "");
	f.LinkRef("enter");
	f.TagText("h4", "ENTER [object]");
	temp = "Attempt to enter the specified object.  If issued from inside "
		"another object, the unit will first leave the object it is "
		"currently in.  The order will only work if the target object is "
		"unoccupied, or is owned by a unit in your faction, or is owned by "
		"a unit which has declared you Friendly.";
	f.Paragraph(temp);
	f.Paragraph("Example:");
	temp = "Enter ship number 114.";
	temp2 = "ENTER 114";
	f.CommandExample(temp, temp2);

	if (!(SkillDefs[S_ENTERTAINMENT].flags & SkillType::DISABLED)) {
		f.ClassTagText("div", "rule", "");
		f.LinkRef("entertain");
		f.TagText("h4", "ENTERTAIN");
		temp = "Spend the month entertaining the populace to earn money.";
		f.Paragraph(temp);
		f.Paragraph("Example:");
		temp = "Entertain for money.";
		temp2 = "ENTERTAIN";
		f.CommandExample(temp, temp2);
	}

	f.ClassTagText("div", "rule", "");
	f.LinkRef("evict");
	f.TagText("h4", "EVICT [unit] ...");
	temp = "Evict the specified unit from the object of which you are "
		"currently the owner.  If multipe EVICT orders are given, all "
		"of the units will be evicted.";
	f.Paragraph(temp);
	f.Paragraph("Example:");
	temp = "Evict units 415 and 698 from an object that this unit owns.";
	temp2 = "EVICT 415 698";
	f.CommandExample(temp, temp2);
	temp = "or";
	temp2 = "EVICT 415\nEVICT 698";
	f.CommandExample(temp, temp2);

	f.ClassTagText("div", "rule", "");
	f.LinkRef("exchange");
	f.TagText("h4", "EXCHANGE [unit] [quantity given] [item given] "
			"[quantity expected] [item expected]");
	temp = "This order allows any two units that can see each other, to "
		"trade items regardless of faction stances.  The orders given by "
		"the two units must be complementary.  If either unit involved does "
		"not have the items it is offering, or if the exchange orders given "
		"are not complementary, the exchange is aborted.  Men may not be "
		"exchanged.";
	f.Paragraph(temp);
	f.Paragraph("Example:");
	temp = "Exchange 10 LBOW for 10 SWOR with unit 1310";
	temp2 = "EXCHANGE 1310 10 LBOW 10 SWOR";
	f.CommandExample(temp, temp2);
	temp = "Unit 1310 would issue (assuming the other unit is 3453)";
	temp2 = "EXCHANGE 3453 10 SWOR 10 LBOW";
	f.CommandExample(temp, temp2);

	if(Globals->FACTION_LIMIT_TYPE == GameDefs::FACLIM_FACTION_TYPES) {
		f.ClassTagText("div", "rule", "");
		f.LinkRef("faction");
		f.TagText("h4", "FACTION [type] [points] ...");
		temp = "Attempt to change your faction's type.  In the order, you "
			"can specify up to three faction types (WAR, TRADE, and MAGIC) "
			"and the number of faction points to assign to each type; if "
			"you are assigning points to only one or two types, you may "
			"omit the types that will not have any points.";
		f.Paragraph(temp);
		temp = "Changing the number of faction points assigned to MAGIC may "
			"be tricky. Increasing the MAGIC points will always succeed, but "
			"if you decrease the number of points assigned to MAGIC, you "
			"must make sure that you have only the number of magic-skilled "
			"leaders allowed by the new number of MAGIC points BEFORE you "
			"change your point distribution. For example, if you have 3 "
			"mages (3 points assigned to MAGIC), but want to use one of "
			"those points for WAR or TRADE (change to MAGIC 2), you must "
			"first get rid of one of your mages by either giving it to "
			"another faction or ordering it to ";
		temp += f.Link("#forget", "FORGET") + " all its magic skills. ";
		temp += "If you have too many mages for the number of points you "
			"try to assign to MAGIC, the FACTION order will fail.";
		if (qm_exist) {
			temp += " Similar problems could occur with TRADE points and "
				"the number of quartermasters controlled by the faction.";
		}
		f.Paragraph(temp);
		f.Paragraph("Examples:");
		temp = "Assign 2 faction points to WAR, 2 to TRADE, and 1 to MAGIC.";
		temp2 = "FACTION WAR 2 TRADE 2 MAGIC 1";
		f.CommandExample(temp, temp2);
		temp = "Become a pure magic faction (assign all points to magic).";
		temp2 = "FACTION MAGIC ";
		temp2 += Globals->FACTION_POINTS;
		f.CommandExample(temp, temp2);
	}

	f.ClassTagText("div", "rule", "");
	f.LinkRef("fightas");
	f.TagText("h4", "FIGHTAS [type]");
	f.TagText("h4", "FIGHTAS");
	temp = "The FIGHTAS order commands a unit to fight in battle as "
		"though it has less mobility than it does. The valid values "
        "for type are 'FOOT', 'RIDE', or 'FLY'. A unit fighting as if it "
        "is on foot will still get combat bonuses from being mounted, "
        "but will fight alongside the army's foottroops. 'FIGHTAS RIDE' "
        "will only affect flying troops, which will fight as though they "
        "are able to ride but not fly. 'FIGHTAS FLY' is equivalent to the "
        "second form, and will cause a unit to fight using its full "
        "mobility.";
	f.Paragraph(temp);
	f.Paragraph("Example:");
	temp = "Set a unit to fight as though it is on foot:";
	temp2 = "FIGHTAS FOOT";
	f.CommandExample(temp, temp2);

	f.ClassTagText("div", "rule", "");
	f.LinkRef("find");
	f.TagText("h4", "FIND [faction]");
	f.TagText("h4", "FIND ALL");
	temp = "Find the email address of the specified faction or of all "
		"factions.";
	f.Paragraph(temp);
	f.Paragraph("Example:");
	temp = "Find the email address of faction 4.";
	temp2 = "FIND 4";
	f.CommandExample(temp, temp2);

	f.ClassTagText("div", "rule", "");
	f.LinkRef("follow");
	f.TagText("h4", "FOLLOW SHIP [number]");
	f.TagText("h4", "FOLLOW UNIT [unit]");
	temp = "Attempt to follow the target unit or ship. If the unit issuing this order is the "
        "owner of a ship (during the move phase) then he will attempt to follow the target by "
        "sailing the ship in the direction the target moves; otherwise, he will MOVE or "
        "ADVANCE in this direction. If the target is not present or moving in a particular "
        "move phase, then the unit will wait, and if the target departs during a later move phase, "
        "then following unit will suffer a penalty to his movepoints equal to the number of movepoints the "
        "target unit has used, and then follow the target to the best of his ability. If the unit "
        "being followed escapes by moving while the follower cannot, then the follower will stop "
        "in the last region where he saw the target unit, and not attempt to move further. "
        "If the target unit is from the follower's or an allied faction, then the following "
        "unit will use the ADVANCE order if the unit he is following is advancing, otherwise he "
        "will simply MOVE. If this order is used to move an army together, it is recommended that "
        "the unit followed should be one of the slowest units in the army, otherwise the army will "
        "get split up.";
	f.Paragraph(temp);
	f.Paragraph("Examples:");
	temp = "Follow ship number 115";
	temp2 = "FOLLOW SHIP 115";
	f.CommandExample(temp, temp2);
	temp = "Follow newly formed unit, alias 3";
	temp2 = "FOLLOW UNIT NEW 3";
	f.CommandExample(temp, temp2);
	
	f.ClassTagText("div", "rule", "");
	f.LinkRef("forget");
	f.TagText("h4", "FORGET [skill]");
	temp = "Forget the given skill. This order is useful for normal units "
		"who wish to learn a new skill, but already know a different skill.";
	f.Paragraph(temp);
	f.Paragraph("Example:");
	temp = "Forget knowledge of Mining.";
	temp2 = "FORGET Mining";
	f.CommandExample(temp, temp2);

	f.ClassTagText("div", "rule", "");
	f.LinkRef("form");
	f.TagText("h4", "FORM [alias]");
	temp = "Form a new unit.  The newly created unit will be in your "
		"faction, in the same region as the unit which formed it, and in "
		"the same structure if any.  It will start off, however, with no "
		"people or items; you should, in the same month, issue orders to "
		"transfer people into the new unit, or have it recruit members. The "
		"new unit will inherit its flags from the unit that forms it, such "
		"as avoiding, behind, and autotax.";
	f.Paragraph(temp);
	temp = "The FORM order is followed by a list of orders for the newly "
		"created unit.  This list is terminated by the END keyword, after "
		"which orders for the original unit resume.";
	f.Paragraph(temp);
	temp = "The purpose of the \"alias\" parameter is so that you can refer "
		"to the new unit. You will not know the new unit's number until "
		"you receive the next turn report.  To refer to the new unit in "
		"this set of orders, pick an alias number (the only restriction on "
		"this is that it must be at least 1, and you should not create two "
		"units in the same region in the same month, with the same alias "
		"numbers).  The new unit can then be referred to as NEW <alias> in "
		"place of the regular unit number.";
	f.Paragraph(temp);
	temp = "You can refer to newly created units belonging to other "
		"factions, if you know what alias number they are, e.g. FACTION 15 "
		"NEW 2 will refer to faction 15's newly created unit with alias 2.";
	f.Paragraph(temp);
	temp = "Note: If a unit moves out of the region in which it was formed "
		"(by the ";
	temp += f.Link("#move", "MOVE") + " order, or otherwise), the alias "
		"will no longer work. This is to prevent conflicts with other units "
		"that may have the same alias in other regions.";
	f.Paragraph(temp);
	temp = "If the demand for recruits in that region that month is much "
		"higher than the supply, it may happen that the new unit does not "
		"gain all the recruits you ordered it to buy, or it may not gain "
		"any recruits at all.  If the new units gains at least one recruit, "
		"the unit will form possessing any unused silver and all the other "
		"items it was given.  If no recruits are gained at all, the empty "
		"unit will be dissolved, and the silver and any other items it was "
		"given will revert to the first unit you have in that region.";
	f.Paragraph(temp);
	f.Paragraph("Example:");
	temp = "This set of orders for unit 17 would create two new units with "
		"alias numbers 1 and 2, name them Merlin's Guards and Merlin's "
		"Workers, set the description for Merlin's Workers, have both units "
		"recruit men, and have Merlin's Guards study combat.  Merlin's "
		"Workers will have the default order ";
	temp += f.Link("#work", "WORK") + ", as all newly created units do. The "
		"unit that created these two then pays them enough money (using the "
		"NEW keyword to refer to them by alias numbers) to cover the costs "
		"of recruitment and the month's maintenance.";
	temp2 = "UNIT 17\n";
	temp2 += "FORM 1\n";
	temp2 += "    NAME UNIT \"Merlin's Guards\"\n";
	if(Globals->RACES_EXIST)
		temp2 += "    BUY 5 Plainsmen\n";
	else
		temp2 += "    BUY 5 men\n";
	temp2 += "    STUDY COMBAT\n";
	temp2 += "END\n";
	temp2 += "FORM 2\n";
	temp2 += "    NAME UNIT \"Merlin's Workers\"\n";
	temp2 += "    DESCRIBE UNIT \"wearing dirty overalls\"\n";
	if(Globals->RACES_EXIST)
		temp2 += "    BUY 15 Plainsmen\n";
	else
		temp2 += "    BUY 15 men\n";
	temp2 += "END\n";
	temp2 += "CLAIM 2500\n";
	temp2 += "GIVE NEW 1 1000 silver\n";
	temp2 += "GIVE NEW 2 2000 silver\n";
	f.CommandExample(temp,temp2);

	f.ClassTagText("div", "rule", "");
	f.LinkRef("give");
	f.TagText("h4", "GIVE [unit] [quantity] [item]");
	f.TagText("h4", "GIVE [unit] ALL [item]");
	f.TagText("h4", "GIVE [unit] ALL [item] EXCEPT [quantity]");
	f.TagText("h4", "GIVE [unit] ALL [item class]");
	f.TagText("h4", "GIVE [unit] UNIT");
	temp = "The first form of the GIVE order gives a quantity of an item to "
		"another unit. The second form of the GIVE order will give all of "
		"a given item to another unit.  The third form will give all of an "
		"item except for a specific quantity to another unit.  The fourth "
		"form will give all items of a specific type to another unit.  The "
		"final form of the GIVE order gives the entire unit to the "
		"specified unit's faction.";
	f.Paragraph(temp);
	temp = "The classes of items which are acceptable for the fourth form of "
		"this order are, NORMAL, ADVANCED, TRADE, MAN or MEN, MONSTER or "
		"MONSTERS, MAGIC, WEAPON OR WEAPONS, ARMOUR, MOUNT or MOUNTS, BATTLE, "
		"SPECIAL, TOOL or TOOLS, FOOD, and ITEM or ITEMS (which is the "
		"combination of all of the previous categories).";
	f.Paragraph(temp);
	temp = "A unit may only give items, including silver, to a unit which "
		"it is able to see, unless the faction of the target unit has "
		"declared you Friendly or better.  If the target unit is not a "
		"member of your faction, then its faction must have declared you "
		"Friendly, with a couple of exceptions. First, silver may be given "
		"to any visible unit, regardless of factional affiliation. Secondly, men "
		"may not be given to units in other factions (you must give the "
		"entire unit); the reason for this is to prevent highly skilled "
		"units from being sabotaged with a ";
	temp += f.Link("#give", "GIVE") + " order.";
	f.Paragraph(temp);
	temp = "There are also a few restrictions on orders given by units who are "
		"being given to another faction. If the receiving faction is not "
		"allied to the giving faction, the unit may not issue the ";
	temp += f.Link("#advance", "ADVANCE") + " order, or issue any more ";
	temp += f.Link("#give", "GIVE") + " orders.  Both of these rules are to "
		"prevent unfair sabotage tactics.";
	f.Paragraph(temp);
	temp = "If 0 is specified as the unit number, then the items are "
		"discarded.";
	f.Paragraph(temp);
	f.Paragraph("Examples:");
	temp = "Give 10 swords to unit 4573.";
	temp2 = "GIVE 4573 10 swords";
	f.CommandExample(temp, temp2);
	temp = "Give 5 chain armour to the new unit, alias 2, belonging to "
		"faction 14.";
	temp2 = "GIVE FACTION 14 NEW 2 5 \"Chain armour\"";
	f.CommandExample(temp, temp2);
	temp = "Give control of this unit to the faction owning unit 75.";
	temp2 = "GIVE 75 UNIT";
	f.CommandExample(temp, temp2);

	f.ClassTagText("div", "rule", "");
	f.LinkRef("guard");
	f.TagText("h4", "GUARD [flag]");
	temp = "GUARD 1 sets the unit issuing the order to prevent non-Friendly "
		"units from collecting taxes in the region, and to prevent any "
		"units not your own from pillaging the region.  Guarding units "
		"will also attempt to prevent Unfriendly units from entering the "
		"region.  GUARD 0 cancels Guard status.";
	f.Paragraph(temp);
	temp = "The Guard and Avoid Combat flags are mutually exclusive; "
		"setting one automatically cancels the other.";
	f.Paragraph(temp);
	f.Paragraph("Example:");
	temp = "Instruct the current unit to be on guard.";
	temp2 = "GUARD 1";
	f.CommandExample(temp, temp2);

	f.ClassTagText("div", "rule", "");
	f.LinkRef("hold");
	f.TagText("h4", "HOLD [flag]");
	temp = "HOLD 1 instructs the issuing unit to never join a battle in "
		"regions the unit is not in.  This can be useful if the unit is in "
		"a building, and doesn't want to leave the building to join combat. "
		"HOLD 0 cancels holding status.";
	f.Paragraph(temp);
	f.Paragraph("Example:");
	temp = "Instruct the unit to avoid combat in other regions.";
	temp2 = "HOLD 1";
	f.CommandExample(temp, temp2);

	f.ClassTagText("div", "rule", "");
	f.LinkRef("leave");
	f.TagText("h4", "LEAVE");
	temp = "Leave the object you are currently in.";
	if(move_over_water) {
		temp += " If a unit is capable of swimming ";
		if(Globals->FLIGHT_OVER_WATER != GameDefs::WFLIGHT_NONE)
			temp += "or flying ";
		temp += "then this order is usable to leave a boat while at sea.";
	} else
		temp += " The order cannot be used at sea.";
	f.Paragraph(temp);
	f.Paragraph("Example:");
	temp = "Leave the current object";
	temp2 = "LEAVE";
	f.CommandExample(temp, temp2);
	
	if(Globals->EARTHSEA_VICTORY) {
    	f.ClassTagText("div", "rule", "");
    	f.LinkRef("master");
    	f.TagText("h4", "MASTER [flag]");
    	temp = "MASTER 0 instructs a mage unit to relinquish any claim to mastery "
            "which they hold. MASTER 1 is a month long order, and causes a mage "
            "to spend the month in meditation, to assume the mantle of master "
            "of whichever field of magic the mage specialises in. The mage "
            "must specialise in one magic field, there may be no more than one other "
            "mage in the world who has previously claimed the title of master "
            "in that magic field, and the mage must have the appropriate skills "
            "necessary to claim mastery. The title of master brings with it a 10% bonus "
            "to the mage's energy recharge, but that mage's location will "
            "forever after be revealed to the world.";
    	f.Paragraph(temp);
    	f.Paragraph("Example:");
    	temp = "Spend the month assuming the mantle of a master";
    	temp2 = "MASTER 1";
    	f.CommandExample(temp, temp2);
	}
	f.ClassTagText("div", "rule", "");
	f.LinkRef("move");
	f.TagText("h4", "MOVE [dir] ...");
	temp = "Attempt to move in the direction(s) specified.  If more than "
		"one direction is given, the unit will move multiple times, in "
		"the order specified by the MOVE order, until no more directions "
		"are given, or until one of the moves fails.  A move can fail "
		"because the units runs out of movement points, because the unit "
		"attempts to move into the ocean, or because the units attempts "
		"to enter a structure, and is rejected.";
	f.Paragraph(temp);
	temp = "Valid directions are:";
	f.Paragraph(temp);
	temp = "1) The compass directions North, Northwest, Southwest, South, "
		"Southeast, and Northeast.  These can be abbreviated N, NW, SW, S, "
		"SE, NE.";
	f.Paragraph(temp);
	temp = "2) A structure number.";
	f.Paragraph(temp);
	temp = "3) OUT, which will leave the structure that the unit is in.";
	f.Paragraph(temp);
	temp = "4) IN, which will move through an inner passage in the "
		"structure that the unit is currently in.";
	f.Paragraph(temp);
	temp = "Multiple MOVE orders given by one unit will chain together.";
	f.Paragraph(temp);
	temp = "Note that MOVE orders can lead to combat, due to hostile units "
		"meeting, or due to an advancing unit being forbidden access to a "
		"region.  In this case, combat occurs each time all movement out "
		"of a single region occurs.";
	f.Paragraph(temp);
	temp = "Example 1: Units 1 and 2 are in Region A, and unit 3 is in "
		"Region B.  Units 1 and 2 are hostile to unit 3.  Both unit 1 and "
		"2 move into region B, and attack unit 3.  Since both units moved "
		"out of the same region, they attack unit 3 at the same time, and "
		"the battle is between units 1 and 2, and unit 3.";
	f.Paragraph(temp);
	temp = "Example 2: Same as example 1, except unit 2 is in Region C, "
		"instead of region A.  Both units move into Region B, and attack "
		"unit 3.  Since unit 1 and unit 2 moved out of different regions, "
		"their battles occur at different times.  Thus, unit 1 attacks unit "
		"3 first, and then unit 2 attacks unit 3 (assuming unit 3 survives "
		"the first attack).  Note that the order of battles could have "
		"happened either way.";
	f.Paragraph(temp);
	f.Paragraph("Examples:");
	temp = "Move N, NE and In";
	temp2 = "MOVE N\nMOVE NE IN";
	f.CommandExample(temp, temp2);
	temp = "or:";
	temp2 = "MOVE N NE IN";
	f.CommandExample(temp, temp2);

	f.ClassTagText("div", "rule", "");
	f.LinkRef("name");
	f.TagText("h4", "NAME UNIT [new name]");
	f.TagText("h4", "NAME FACTION [new name]");
	f.TagText("h4", "NAME OBJECT [new name]");
	if (Globals->TOWNS_EXIST)
		f.TagText("h4", "NAME CITY [new name]");
	temp = "Change the name of the unit, or of your faction, or of "
		"the object the unit is in (of which the unit must be the owner). "
		"Names can be of any length, up to the line length your mailer "
		"can handle.  Names may not contain parentheses (square brackets "
		"can be used instead if necessary), or any control characters.";
	f.Paragraph(temp);
	if (Globals->TOWNS_EXIST) {
		temp = "In order to rename a settlement (city, town or village), "
			"the unit attempting to rename it must be the owner of a large "
			"enough structure located in the city. It requires a tower or "
			"better to rename a village, a fort or better to rename a town "
			"and a castle or mystic fortress to rename a city. ";
		if (Globals->CITY_RENAME_COST) {
			int c=Globals->CITY_RENAME_COST;
			temp += AString("It also costs $") + c + " to rename a village, $";
			temp += AString(2*c) + " to rename a town, and $";
			temp += AString(3*c) + " to rename a city.";
		}
		f.Paragraph(temp);
	}
	f.Paragraph("Example:");
	temp = "Name your faction \"The Merry Pranksters\".";
	temp2 = "NAME FACTION \"The Merry Pranksters\"";
	f.CommandExample(temp, temp2);

	f.ClassTagText("div", "rule", "");
	f.LinkRef("noaid");
	f.TagText("h4", "NOAID [flag]");
	temp = "NOAID 1 indicates that if the unit attacks, or is attacked, it "
		"is not to be aided by units in other hexes. NOAID status is very "
		"useful for scouts or probing units, who do not wish to drag "
		"their nearby armies into battle if they are caught. NOAID 0 "
		"cancels this.";
	f.Paragraph(temp);
	temp = "If multiple units are on one side in a battle, they must all "
		"have the NOAID flag on, or they will receive aid from other hexes.";
	f.Paragraph(temp);
	f.Paragraph("Example:");
	temp = "Set a unit to receive no aid in battle.";
	temp2 = "NOAID 1";
	f.CommandExample(temp, temp2);

	if(move_over_water) {
		f.ClassTagText("div", "rule", "");
		f.LinkRef("nocross");
		f.TagText("h4", "NOCROSS [flag]");
		temp = "NOCROSS 1 indicates that if a unit attempts to cross a "
			"body of water then that unit should instead not cross it, "
			"regardless of whether the unit otherwise could do so. ";
		if(may_sail) {
			temp += "Units inside of a ship are not affected by this flag "
				"(IE, they are able to sail within the ship). ";
		}
		temp += "This flag is useful to prevent scouts from accidentally "
			"drowning when exploring in games where movement over water "
			"is allowed. NOCROSS 0 cancels this.";
		f.Paragraph(temp);
		f.Paragraph("Example:");
		temp = "Set a unit to not permit itself to cross water.";
		temp2 = "NOCROSS 1";
		f.CommandExample(temp, temp2);
	}

	f.ClassTagText("div", "rule", "");
	f.LinkRef("option");
	f.TagText("h4", "OPTION TIMES");
	f.TagText("h4", "OPTION NOTIMES");
	f.TagText("h4", "OPTION TEMPLATE OFF");
	f.TagText("h4", "OPTION TEMPLATE SHORT");
	f.TagText("h4", "OPTION TEMPLATE LONG");
	f.TagText("h4", "OPTION TEMPLATE MAP");
	temp = "The OPTION order is used to toggle various settings that "
		"affect your reports, and other email details. OPTION TIMES sets it "
		"so that your faction receives the times each week (this is the "
		"default); OPTION NOTIMES sets it so that your faction is not sent "
		"the times.";
	f.Paragraph(temp);
	temp = "The OPTION TEMPLATE order toggles the length of the Orders "
		"Template that appears at the bottom of a turn report.  The OFF "
		"setting eliminates the Template altogether, and the SHORT, LONG "
		"and MAP settings control how much detail the Template contains. "
		"The MAP setting will produce an ascii map of the region and "
		"surrounding regions in addition other details.";
	f.Paragraph(temp);
	temp = "For the MAP template, the region identifiers are (there might "
		"be additional symbols for unusual/special terrain):";
	f.Paragraph(temp);
	f.Enclose(1, "table");
	if(Globals->UNDERWORLD_LEVELS) {
		f.Enclose(1, "tr");
		f.Enclose(1, "td align=\"left\" nowrap class=\"fixed\"");
		f.PutStr("####");
		f.Enclose(0, "td");
		f.Enclose(1, "td align=\"left\" nowrap");
		f.PutStr("BLOCKED HEX (Underworld)");
		f.Enclose(0, "td");
		f.Enclose(0, "tr");
	}
	f.Enclose(1, "tr");
	f.Enclose(1, "td align=\"left\" nowrap class=\"fixed\"");
	f.PutStr("~~~~");
	f.Enclose(0, "td");
	f.Enclose(1, "td align=\"left\" nowrap");
	f.PutStr("OCEAN HEX");
	f.Enclose(0, "td");
	f.Enclose(0, "tr");
	f.Enclose(1, "tr");
	f.Enclose(1, "td align=\"left\" nowrap class=\"fixed\"");
	f.PutStr("    ");
	f.Enclose(0, "td");
	f.Enclose(1, "td align=\"left\" nowrap");
	temp = "PLAINS";
	if(Globals->UNDERWORLD_LEVELS)
		temp += "/TUNNELS";
	temp += " HEX";
	f.PutStr(temp);
	f.Enclose(0, "td");
	f.Enclose(0, "tr");
	f.Enclose(1, "tr");
	f.Enclose(1, "td align=\"left\" nowrap class=\"fixed\"");
	f.PutStr("^^^^");
	f.Enclose(0, "td");
	f.Enclose(1, "td align=\"left\" nowrap");
	temp = "FOREST";
	if(Globals->UNDERWORLD_LEVELS)
		temp += "/UNDERFOREST";
	temp += " HEX";
	f.PutStr(temp);
	f.Enclose(0, "td");
	f.Enclose(0, "tr");
	f.Enclose(1, "tr");
	f.Enclose(1, "td align=\"left\" nowrap class=\"fixed\"");
	f.PutStr("/\\/\\");
	f.Enclose(0, "td");
	f.Enclose(1, "td align=\"left\" nowrap");
	f.PutStr("MOUNTAIN HEX");
	f.Enclose(0, "td");
	f.Enclose(0, "tr");
	f.Enclose(1, "tr");
	f.Enclose(1, "td align=\"left\" nowrap class=\"fixed\"");
	f.PutStr("vvvv");
	f.Enclose(0, "td");
	f.Enclose(1, "td align=\"left\" nowrap");
	f.PutStr("SWAMP HEX");
	f.Enclose(0, "td");
	f.Enclose(0, "tr");
	if(!Globals->CONQUEST_GAME) {
		f.Enclose(1, "tr");
		f.Enclose(1, "td align=\"left\" nowrap class=\"fixed\"");
		f.PutStr("@@@@");
		f.Enclose(0, "td");
		f.Enclose(1, "td align=\"left\" nowrap");
		f.PutStr("JUNGLE HEX");
		f.Enclose(0, "td");
		f.Enclose(0, "tr");
		f.Enclose(1, "tr");
		f.Enclose(1, "td align=\"left\" nowrap class=\"fixed\"");
		f.PutStr("....");
		f.Enclose(0, "td");
		f.Enclose(1, "td align=\"left\" nowrap");
		temp = "DESERT";
		if(Globals->UNDERWORLD_LEVELS)
			temp += "/CAVERN";
		temp += " HEX";
		f.PutStr(temp);
		f.Enclose(0, "td");
		f.Enclose(0, "tr");
		f.Enclose(1, "tr");
		f.Enclose(1, "td align=\"left\" nowrap class=\"fixed\"");
		f.PutStr(",,,,");
		f.Enclose(0, "td");
		f.Enclose(1, "td align=\"left\" nowrap");
		f.PutStr("TUNDRA HEX");
		f.Enclose(0, "td");
		f.Enclose(0, "tr");
	}
	if(Globals->NEXUS_EXISTS) {
		f.Enclose(1, "tr");
		f.Enclose(1, "td align=\"left\" nowrap class=\"fixed\"");
		f.PutStr("!!!!");
		f.Enclose(0, "td");
		f.Enclose(1, "td align=\"left\" nowrap");
		f.PutStr("THE NEXUS");
		f.Enclose(0, "td");
		f.Enclose(0, "tr");
	}
	f.Enclose(0, "table");
	f.Paragraph("Example:");
	temp = "Set your faction to recieve the map format order template";
	temp2 = "OPTION TEMPLATE MAP";
	f.CommandExample(temp, temp2);

	f.ClassTagText("div", "rule", "");
	f.LinkRef("password");
	f.TagText("h4", "PASSWORD [password]");
	f.TagText("h4", "PASSWORD");
	temp = "The PASSWORD order is used to set your faction's password. If "
		"you have a password set, you must specify it on your #ATLANTIS "
		"line for the game to accept your orders.  This protects you orders "
		"from being overwritten, either by accident or intentionally by "
		"other players.  PASSWORD with no password given clears out your "
		"faction's password.";
	f.Paragraph(temp);
	temp = "IMPORTANT: The PASSWORD order does not take effect until the "
		"turn is actually run.  So if you set your password, and then want "
		"to re-submit orders, you should use the old password until the "
		"turn has been run.";
	f.Paragraph(temp);
	f.Paragraph("Example:");
	temp = "Set the password to \"xyzzy\".";
	temp2 = "PASSWORD xyzzy";
	f.CommandExample(temp, temp2);

	f.ClassTagText("div", "rule", "");
	f.LinkRef("pillage");
	f.TagText("h4", "PILLAGE");
	temp = "Use force to extort as much money as possible from the region. "
		"Note that the ";
	temp += f.Link("#tax", "TAX") + " order and the PILLAGE order are ";
	temp += "mutually exclusive; a unit may only attempt to do one in a "
		"turn.";
	f.Paragraph(temp);
	f.Paragraph("Example:");
	temp = "Pillage the current hex.";
	temp2 = "PILLAGE";
	f.CommandExample(temp, temp2);

	if(Globals->USE_PREPARE_COMMAND) {
		f.ClassTagText("div", "rule", "");
		f.LinkRef("prepare");
		f.TagText("h4", "PREPARE [item]");
		temp = "This command allows a mage or apprentice to prepare a "
			"battle item (e.g. a Staff of Fire) for use in battle. ";
		if (Globals->USE_PREPARE_COMMAND == GameDefs::PREPARE_STRICT) {
			temp += " This selects the battle item which will be used, ";
		} else {
			temp += "This allows the mage to override the usual selection "
				"of battle items, ";
		}
		temp += "and also cancels any spells set via the ";
		temp += f.Link("#combat", "COMBAT") + " order.";
		f.Paragraph(temp);
		f.Paragraph("Example:");
		temp = "Select a staff of fire as the ";
		if (!(Globals->USE_PREPARE_COMMAND == GameDefs::PREPARE_STRICT))
			temp += "preferred ";
		temp += "battle item.";
		temp2 = "PREPARE STAF";
		f.CommandExample(temp, temp2);
	}

	f.ClassTagText("div", "rule", "");
	f.LinkRef("produce");
	f.TagText("h4", "PRODUCE [item]");
	temp = "Spend the month producing as much as possible of the specified "
		"item.";
	f.Paragraph(temp);
	f.Paragraph("Example:");
	temp = "Produce as many crossbows as possible.";
	temp2 = "PRODUCE crossbows";
	f.CommandExample(temp, temp2);

	f.ClassTagText("div", "rule", "");
	f.LinkRef("promote");
	f.TagText("h4", "PROMOTE [unit]");
	temp = "Promote the specified unit to owner of the object of which you "
		"are currently the owner.  The target unit must have declared you "
		"Friendly.";
	f.Paragraph(temp);
	f.Paragraph("Example:");
	temp = "Promote unit 415 to be the owner of the object that this unit "
		"owns.";
	temp2 = "PROMOTE 415";
	f.CommandExample(temp, temp2);

	f.ClassTagText("div", "rule", "");
	f.LinkRef("quit");
	f.TagText("h4", "QUIT [password]");
	temp = "Quit the game.  On issuing this order, your faction will be "
		"completely and permanently destroyed. Note that you must give "
		"your password for the quit order to work; this is to provide "
		"some safety against accidentally issuing this order.";
	f.Paragraph(temp);
	f.Paragraph("Example:");
	temp = "Quit the game if your password is foobar.";
	temp2 = "QUIT \"foobar\"";
	f.CommandExample(temp, temp2);

	f.ClassTagText("div", "rule", "");
	f.LinkRef("restart");
	f.TagText("h4", "RESTART [password]");
	temp = "Similar to the ";
	temp += f.Link("#quit", "QUIT") + " order, this order will completely "
		"and permanently destroy your faction. However, it will begin a "
		"brand new faction for you (you will get a separate turn report for "
		"the new faction). Note that you must give your password for this "
		"order to work, to provide some protection against accidentally "
		"issuing this order.";
	f.Paragraph(temp);
	f.Paragraph("Example:");
	temp = "Restart as a new faction if your password is foobar.";
	temp2 = "RESTART \"foobar\"";
	f.CommandExample(temp, temp2);

	f.ClassTagText("div", "rule", "");
	f.LinkRef("reveal");
	f.TagText("h4", "REVEAL");
	f.TagText("h4", "REVEAL UNIT");
	f.TagText("h4", "REVEAL FACTION");
	temp = "Cause the unit to either show itself (REVEAL UNIT), or show "
		"itself and its faction affiliation (REVEAL FACTION), in the turn "
		"report, to all other factions in the region. ";
	if(has_stea) {
		temp += "Used to reveal high stealth scouts, should there be some "
			"reason to. ";
	}
	temp += "REVEAL is used to cancel this.";
	f.Paragraph(temp);
	f.Paragraph("Examples:");
	temp = "Show the unit to all factions.";
	temp2 = "REVEAL UNIT";
	f.CommandExample(temp, temp2);
	temp = "Show the unit and it's affiliation to all factions.";
	temp2 = "REVEAL FACTION";
	f.CommandExample(temp, temp2);
	temp = "Cancels revealling.";
	temp2 = "REVEAL";
	f.CommandExample(temp, temp2);

	if(may_sail) {
		f.ClassTagText("div", "rule", "");
		f.LinkRef("sail");
		f.TagText("h4", "SAIL [dir] ...");
		f.TagText("h4", "SAIL");
		temp = "The first form will sail the ship, which the unit must be "
			"the owner of, in the directions given.  The second form "
			"will cause the unit to aid in the sailing of the ship, using "
			"the Sailing skill.  See the section on movement for more "
			"information on the mechanics of sailing.";
		f.Paragraph(temp);
		f.Paragraph("Example:");
		temp = "Sail north, then northwest.";
		temp2 = "SAIL N NW";
		f.CommandExample(temp, temp2);
		temp = "or:";
		temp2 = "SAIL N\nSAIL NW";
		f.CommandExample(temp, temp2);
	}

	if(Globals->TOWNS_EXIST) {
		f.ClassTagText("div", "rule", "");
		f.LinkRef("sell");
		f.TagText("h4", "SELL [quantity] [item]");
		f.TagText("h4", "SELL ALL [item]");
		temp = "Attempt to sell the amount given of the item given.  If the "
			"unit does not have as many of the item as it is trying to sell, "
			"it will attempt to sell all that it has. The second form will "
			"attempt to sell all of that item, regardless of how many it has. "
			"If more of the item are on sale (by all the units in the region) "
			"than are wanted by the region, the number sold per unit will be "
			"split up in proportion to the number each unit tried to sell.";
		f.Paragraph(temp);
		f.Paragraph("Example:");
		temp = "Sell 10 furs to the market.";
		temp2 = "SELL 10 furs";
		f.CommandExample(temp, temp2);
	}
    
    if(Globals->SEND_COST >= 0) {
    	f.ClassTagText("div", "rule", "");
    	f.LinkRef("send");
    	f.TagText("h4", "SEND DIRECTION [dir] [quantity] [item]");
    	f.TagText("h4", "SEND UNIT [unit] [quantity] [item]");
    	f.TagText("h4", "SEND DIRECTION [dir] UNIT [unit] [quantity] [item]");
    	f.TagText("h4", "SEND UNIT [unit] ALL [item]");
    	f.TagText("h4", "SEND UNIT [unit] ALL [item] EXCEPT [quantity]");
    	f.TagText("h4", "SEND UNIT [unit] ALL [item class]");
    	temp = "The send order allows goods to be sent to neighbouring hexes. "
            "Using this order will cost 1 silver per weight of the goods SENT, "
            "plus 1 silver for any weightless good SENT. If the neighbouring "
            "hex cannot be reached by a walking unit (eg a forest in winter), "
            "this cost will be doubled. If the sending unit cannot "
            "pay this cost, the order will fail.";
    	f.Paragraph(temp);
    	temp = "The unit to recieve the goods may be specified in three ways. "
            "Firstly, the direction the goods are to be sent may be given, "
            "and they will be transferred to the first unit belonging to your faction "
            "in the neighbouring hex in the direction specified. Valid directions "
            "are: N, NE, SE, S, SW, NW, IN, or the number of an object with an "
            "inner passage. Alternatively, the unit "
            "number may be given, and the goods will be transferred to that unit, "
            "providing it is present in a neighbouring hex (and not via an "
            "inner passage. Finally, both a direction "
            "and unit number may be specified, if required.";
    	f.Paragraph(temp);	
    	
    	temp = "The classes of items which are acceptable for "
    		"this order are NORMAL, ADVANCED, TRADE, "
    		"MAGIC, WEAPON OR WEAPONS, ARMOUR, MOUNT or MOUNTS, BATTLE, "
    		"SPECIAL, TOOL or TOOLS, FOOD, and ITEM or ITEMS (which is the "
    		"combination of all of the previous categories).";
    	f.Paragraph(temp);
    	temp = "A unit may not send any of the following: men, monsters, or illusions. "
    		"If the target unit is not a member of your faction, then its faction "
            "must have declared you Friendly. Lastly, the recieving unit may not "
            "have any MOVE, ADVANCE, FOLLOW or SAIL orders, nor be onboard a ship "
            "which a crew member is attempting to SAIL.";
    	f.Paragraph(temp);
    	f.Paragraph("Examples:");
    	temp = "Send 10 swords to the region to the northwest.";
    	temp2 = "SEND DIRECTION NW 10 swords";
    	f.CommandExample(temp, temp2);
    	temp = "Send 5 wood to unit 1668.";
    	temp2 = "SEND UNIT 1668 5 wood";
    	f.CommandExample(temp, temp2);
    	temp = "Send 5 chain armour to the new unit, alias 2, created in the region directly to the north.";
    	temp2 = "SEND DIRECTION N UNIT NEW 2 5 \"Chain armour\"";
    	f.CommandExample(temp, temp2);
    	temp = "Send all magic items to unit 1668, but only if it is located in the region reached via the inner passage in object number 1.";
    	temp2 = "SEND DIRECTION 1 UNIT 1668 ALL magic";
    	f.CommandExample(temp, temp2);
    }

	f.ClassTagText("div", "rule", "");
	f.LinkRef("show");
	f.TagText("h4", "SHOW SKILL [skill] [level]");
	f.TagText("h4", "SHOW ITEM [item]");
	f.TagText("h4", "SHOW OBJECT [object]");
	temp = "The first form of the order shows the skill description for a "
		"skill that your faction already possesses. The second form "
		"returns some information about an item that is not otherwise "
		"apparent on a report, such as the weight. The last form "
		"returns some information about an object (such as a ship or a "
		"building).";
	f.Paragraph(temp);
	f.Paragraph("Examples:");
	temp = "Show the skill report for Mining 3 again.";
	temp2 = "SHOW SKILL Mining 3";
	f.CommandExample(temp, temp2);
	temp = "Show the item information for swords again.";
	temp2 = "SHOW ITEM sword";
	f.CommandExample(temp, temp2);
	temp = "Show the information for towers again.";
	temp2 = "SHOW OBJECT tower";
	f.CommandExample(temp, temp2);

	f.ClassTagText("div", "rule", "");
	f.LinkRef("spoils");
	f.TagText("h4", "SPOILS [type]");
	f.TagText("h4", "SPOILS");
	temp = "The SPOILS order determines which types of spoils the unit "
		"should take after a battle.  The valid values for type are "
		"'NONE', 'WALK', 'RIDE', 'FLY', or 'ALL'. The second form is "
		"equivalent to 'SPOILS ALL'.";
	f.Paragraph(temp);
	temp = "When this command is issued, only spoils with 0 weight (at "
		"level NONE) or spoils which weigh less than or equal to their "
		"capacity in the specified movement mode (at any level other than "
		"ALL) will be picked up.  SPOILS ALL will allow a unit to collect "
		"any spoils which are dropped regardless of weight or capacity.";
	f.Paragraph(temp);
	f.Paragraph("Example:");
	temp = "Set a unit to only pick up items which have flying capacity";
	temp2 = "SPOILS FLY";
	f.CommandExample(temp, temp2);

	if(has_stea) {
		f.ClassTagText("div", "rule", "");
		f.LinkRef("steal");
		f.TagText("h4", "STEAL [unit] [item]");
		temp = "Attempt to steal as much as possible of the specified "
			"item from the specified unit. The order may only be issued "
			"by a one-man unit.";
		f.Paragraph(temp);
		temp = "A unit may only attempt to steal from a unit which is "
			"able to be seen.";
		f.Paragraph(temp);
		f.Paragraph("Examples:");
		temp = "Steal silver from unit 123.";
		temp2 = "STEAL 123 SILVER";
		f.CommandExample(temp, temp2);
		temp = "Steal wood from unit 321.";
		temp2 = "STEAL 321 wood";
		f.CommandExample(temp, temp2);
	}

	f.ClassTagText("div", "rule", "");
	f.LinkRef("study");
	f.TagText("h4", "STUDY [skill]");
	temp = "Spend the month studying the specified skill.";
	f.Paragraph(temp);
	f.Paragraph("Example:");
	temp = "Study horse training.";
	temp2 = "STUDY \"Horse Training\"";
	f.CommandExample(temp, temp2);

	f.ClassTagText("div", "rule", "");
	f.LinkRef("tactics");
	f.TagText("h4", "TACTICS");
	f.TagText("h4", "TACTICS AGGRESSIVE");
	f.TagText("h4", "TACTICS DEFENSIVE");
	temp = "Used to adjust the behaviour of battle commanders. "
        "Aggressive commanders will be more likely to order "
        "their cavalry to flank, and that cavalry is more likely "
        "to attack enemy ranged formations if it does so. Defensive "
        "commanders will flank more rarely, requiring a more powerful lure "
        "to send any cavalry to try and hit the enemy's backline. "
        "If a unit does not command the battle, then this flag will "
        "have no effect. Using TACTICS without specifying a behaviour will reset "
        "the commander to normal behaviour.";
	f.Paragraph(temp);
	f.Paragraph("Example:");
	temp = "Set a unit to lead aggressively in combat.";
	temp2 = "TACTICS AGGRESSIVE";
	f.CommandExample(temp, temp2);

	f.ClassTagText("div", "rule", "");
	f.LinkRef("tax");
	f.TagText("h4", "TAX");
	temp = "Attempt to collect taxes from the region. ";
	if(Globals->FACTION_LIMIT_TYPE == GameDefs::FACLIM_FACTION_TYPES)
		temp += "Only War factions may collect taxes, and then ";
	else
		temp += "Taxes may be collected ";
	temp += "only if there are no non-Friendly units on guard. Only "
		"combat-ready units may issue this order. Note that the TAX order "
		"and the ";
	temp += f.Link("#pillage", "PILLAGE") + " order are mutually exclusive; "
		"a unit may only attempt to do one in a turn.";
	f.Paragraph(temp);
	f.Paragraph("Example:");
	temp = "Attempt to collect taxes.";
	temp2 = "TAX";
	f.CommandExample(temp, temp2);

	f.ClassTagText("div", "rule", "");
	f.LinkRef("teach");
	f.TagText("h4", "TEACH [unit] ...");
	temp = "Attempt to teach the specified units whatever skill they are "
		"studying that month.  A list of several units may be specified. "
		"All units to be taught must have declared you Friendly. "
		"Subsequent TEACH orders can be used to add units to be taught.";
	f.Paragraph(temp);
	f.Paragraph("Example:");
	temp = "Teach new unit 2 and unit 510 whatever they are studying.";
	temp2 = "TEACH NEW 2 510";
	f.CommandExample(temp, temp2);
	temp = "or:";
	temp2 = "TEACH NEW 2\nTEACH 510";
	f.CommandExample(temp, temp2);

	if (qm_exist) {
		f.ClassTagText("div", "rule", "");
		f.LinkRef("transport");
		f.TagText("h4", "TRANSPORT [unit] [num] [item]");
		f.TagText("h4", "TRANSPORT [unit] ALL [item]");
		f.TagText("h4", "TRANSPORT [unit] ALL [item] EXCEPT [amount]");
		temp = "Transport the specified items to the given target.  In "
			"the second form all of the specified item is transport.  In "
			"the last form, all of the specified item except for the "
			"specified amount is transport.";
		if (Globals->SHIPPING_COST > 0) {
			temp += " Long distance transportation of goods between ";
			temp += Globals->LOCAL_TRANSPORT;
			temp += AString(" and ") + Globals->NONLOCAL_TRANSPORT;
			temp += " hexes away has an associated cost.  This cost is based "
				"on the weight of the items being transported.";
			if (Globals->TRANSPORT & GameDefs::QM_AFFECT_COST) {
				temp += " At higher skill levels of the quartermaster "
					"skill, the cost for transporting goods will be less.";
			}
			if (Globals->TRANSPORT & GameDefs::QM_AFFECT_DIST) {
				temp += " At higher skill levels of the quartermaster "
					"skill, the maximum distance goods can be transported "
					"increases over the above.";
			}
		}
		temp += " The target of the transport unit must be a unit with the "
			"quartermaster skill and must be the owner of a transport "
			"structure.";
		temp += " For long distance transport between quartermasters, the "
			"issuing unit must also be a quartermaster and be the owner of "
			"a transport structure.  Use of this order counts as trade "
			"activity in the hex.";
		f.Paragraph(temp);
		f.Paragraph("Examples:");
		temp = "Transport 10 STON to unit 1234";
		temp2 = "TRANSPORT 1234 10 STON";
		f.CommandExample(temp, temp2);
		temp = "Transport all except 10 SWOR to unit 3432";
		temp2 = "TRANSPORT 3432 ALL SWOR EXCEPT 10";
		f.CommandExample(temp, temp2);
	}

	f.ClassTagText("div", "rule", "");
	f.LinkRef("turn");
	f.TagText("h4", "TURN");
	temp = "The TURN order may be used to delay orders by one (or more) "
		"turns. By making the TURN order repeating (via '@'), orders inside "
		"the TURN/ENDTURN construct will repeat.  Multiple TURN orders in a "
		"row will execute on successive turns, and if they all repeat, they "
		"will form a loop of orders.  Each TURN section must be ended by an "
		"ENDTURN line.";
	f.Paragraph(temp);
	f.Paragraph("Examples:");
	temp = "Study combat this month, move north next month, and then in two "
		"months, pillaging and advance north.";
	temp2 = "STUDY COMB\n";
	temp2 += "TURN\n";
	temp2 += "    MOVE N\n";
	temp2 += "ENDTURN\n";
	temp2 += "TURN\n";
	temp2 += "    PILLAGE\n";
	temp2 += "    ADVANCE N\n";
	temp2 += "ENDTURN";
	f.CommandExample(temp, temp2);
	temp = "After the turn, the orders for that unit would look as "
		"follows in the orders template:";
	temp2 = "MOVE N\n";
	temp2 += "TURN\n";
	temp2 += "    PILLAGE\n";
	temp2 += "    ADVANCE N\n";
	temp2 += "ENDTURN";
	f.CommandExample(temp, temp2);
	temp = "Set up a simple cash caravan (It's assumed here that someone is "
		"funnelling cash into this unit.";
	temp2 = "MOVE N\n";
	temp2 += "@TURN\n";
	temp2 += "    GIVE 13523 1000 SILV\n";
	temp2 += "    MOVE S S S\n";
	temp2 += "ENDTURN\n";
	temp2 += "@TURN\n";
	temp2 += "    MOVE N N N\n";
	temp2 += "ENDTURN";
	f.CommandExample(temp, temp2);
	temp = "After the turn, the orders for that unit would look as "
		"follows in the orders template:";
	temp2 = "GIVE 13523 1000 SILV\n";
	temp2 += "MOVE S S S\n";
	temp2 += "@TURN\n";
	temp2 += "    MOVE N N N\n";
	temp2 += "ENDTURN\n";
	temp2 += "@TURN\n";
	temp2 += "    GIVE 13523 1000 SILV\n";
	temp2 += "    MOVE S S S\n";
	temp2 += "ENDTURN";
	f.CommandExample(temp, temp2);
	temp = "If the unit does not have enough movement points to cover "
			"the full distance, the MOVE commands will automatically "
			"be completed over multiple turns before executing the next "
			"TURN block.";
	f.Paragraph(temp);

	if(Globals->USE_WEAPON_ARMOR_COMMAND) {
		f.ClassTagText("div", "rule", "");
		f.LinkRef("weapon");
		f.TagText("h4", "WEAPON [item] ...");
		f.TagText("h4", "WEAPON");
		temp = "This command allows you to set a list of preferred weapons "
			"for a unit.  After searching for weapons on the preferred "
			"list, the standard weapon precedence takes effect if a weapon "
			"hasn't been set.  The second form clears the preferred weapon "
			"list.";
		f.Paragraph(temp);
		f.Paragraph("Examples");
		temp = "Set the unit to select double bows, then longbows then "
			"crossbows";
		temp2 = "WEAPON DBOW LBOW XBOW";
		f.CommandExample(temp, temp2);
		temp = "Clear the preferred weapon list.";
		temp2 = "WEAPON";
		f.CommandExample(temp, temp2);
	}
	if (Globals->ALLOW_BANK & GameDefs::BANK_ENABLED) {
		f.Enclose(1, "LI");
		f.PutStr("Bank orders.");
		f.Enclose(1, "UL");
		f.TagText("LI" , "Interest is calculated.");
		temp = f.Link("#bank","BANK") + " DEPOSIT orders are processed.";
		f.TagText("LI", temp);
		temp = f.Link("#bank","BANK") + " WITHDRAW orders are processed.";
		f.TagText("LI", temp);
		f.Enclose(0, "UL");
		f.Enclose(0, "LI");
	}

	if(Globals->ALLOW_WITHDRAW) {
		f.ClassTagText("div", "rule", "");
		f.LinkRef("withdraw");
		f.TagText("h4", "WITHDRAW [item]");
		f.TagText("h4", "WITHDRAW [quantity] [item]");
		temp = "Use unclaimed funds to aquire basic items that you need. "
			"If you do not have sufficient unclaimed, or if you try "
			"withdraw any other than a basic item, an error will be given. "
			"Withdraw can NOT be used in the Nexus (to prevent building "
			"towers and such there).  The first form is the same as "
			"WITHDRAW 1 [item] in the second form.";
		f.Paragraph(temp);
		f.Paragraph("Examples:");
		temp = "Withdraw 5 stone.";
		temp2 = "WITHDRAW 5 stone";
		f.CommandExample(temp, temp2);
		temp = "Withdraw 1 iron.";
		temp2 = "WITHDRAW iron";
		f.CommandExample(temp, temp2);
		if(Globals->WISHSKILLS_ENABLED) {	    	
    		f.ClassTagText("div", "rule", "");
    		f.LinkRef("wishdraw");
    		f.TagText("h4", "WISHDRAW [item]");
    		f.TagText("h4", "WISHDRAW [quantity] [item]");
    		temp = "This order is enabled in testgames only. "
    			"It allows you to withdraw most items, without paying the usual cost "
    			"associated with doing so. All items which may be given between units "
                "may be wishdrawn, including some (but not all) monsters. Wishdraw takes place after Withdraw. "
                "Note that units with no men in them may not execute their orders, so a wishdrawing unit should have at least one man at the withdraw phase. Please "
                "note that misuse of this order may create situations which were never "
                "intended to occur in Atlantis, such as mage units with multiple men, or mixed normal races and leaders. "
                "Because it is enabled for testing purposes, please be careful with its "
                "use, and use it only to wishdraw things which could be gained using some "
                "other method.";
    		f.Paragraph(temp);
    		f.Paragraph("Examples:");
    		temp = "Wishdraw 5 stone.";
    		temp2 = "WISHDRAW 5 stone";
    		f.CommandExample(temp, temp2);
    		temp = "Wishdraw 1 mithril sword.";
    		temp2 = "WITHDRAW mithril_sword";
    		f.CommandExample(temp, temp2);
    		
    		f.ClassTagText("div", "rule", "");
    		f.LinkRef("wishskill");
    		f.TagText("h4", "WISHSKILL [skill] [days of knowledge] [days of experience]");
    		temp = "This order is enabled in testgames only. "
    			"It allows you to change the skills your unit has, without paying the usual cost "
    			"associated with doing so. This order should adjust unit to mage / apprentice "
                "status if appropriate, and limit skill levels to the maximum allowed for the unit, "
                "but it makes no guarantee to do so in all circumstances - so as with Wishdraw, use "
                "this order with caution. Usual prerequisites required for skills are ignored "
                "in the execution of this order. Because it is enabled for testing purposes, please be careful with its "
                "use, and use it only to wishskill skills which could be gained using some "
                "other method in the game.";
    		f.Paragraph(temp);
    		f.Paragraph("Examples:");
    		temp = "Wishskill level 5 mining.";
    		temp2 = "WISHSKILL mining 180 90";
    		f.CommandExample(temp, temp2);
    		temp = "Wishskill level 4 summon wind.";
    		temp2 = "WISHSKILL summon_wind 90 90";
    		f.CommandExample(temp, temp2);
		}
	}

	f.ClassTagText("div", "rule", "");
	f.LinkRef("work");
	f.TagText("h4", "WORK");
	temp = "Spend the month performing manual work for wages.";
	f.Paragraph(temp);
	f.Paragraph("Example:");
	temp = "Work all month.";
	temp2 = "WORK";
	f.CommandExample(temp, temp2);

	f.LinkRef("sequenceofevents");
	f.ClassTagText("div", "rule", "");
	f.TagText("h2", "Sequence of Events");
	temp = "Each turn, the following sequence of events occurs:";
	f.Paragraph(temp);
	f.Enclose(1, "OL");
	f.Enclose(1, "li");
	f.PutStr("Instant orders.");
	f.Enclose(1, "ul");
	temp = f.Link("#turn", "TURN") + " orders are processed.";
	f.TagText("li", temp);
	temp = f.Link("#form", "FORM") + " orders are processed.";
	f.TagText("li", temp);
	temp = f.Link("#address", "ADDRESS") + ", ";
	if(Globals->USE_WEAPON_ARMOR_COMMAND)
		temp += f.Link("#armour", "ARMOUR") + ", ";
	temp += f.Link("#autotax", "AUTOTAX") + ", ";
	temp += f.Link("#avoid", "AVOID") + ", ";
	temp += f.Link("#behind", "BEHIND") + ", ";
	temp += f.Link("#claim", "CLAIM") + ", ";
	temp += f.Link("#combat", "COMBAT") + ", ";
	if(Globals->FOOD_ITEMS_EXIST)
		temp += f.Link("#consume", "CONSUME") + ", ";
	temp += f.Link("#declare", "DECLARE") + ", ";
	temp += f.Link("#describe", "DESCRIBE") + ", ";
	if(Globals->FACTION_LIMIT_TYPE == GameDefs::FACLIM_FACTION_TYPES)
		temp += f.Link("#faction", "FACTION") + ", ";
	temp += f.Link("#fightas", "FIGHTAS") + " 0, ";
	temp += f.Link("#guard", "GUARD") + " 0, ";
	temp += f.Link("#hold", "HOLD") + ", ";
	temp += f.Link("#name", "NAME") + ", ";
	temp += f.Link("#noaid", "NOAID") + ", ";
	if(move_over_water)
		temp += f.Link("#nocross", "NOCROSS") + ", ";
	temp += f.Link("#option", "OPTION") + ", ";
	temp += f.Link("#password", "PASSWORD") + ", ";
	if(Globals->USE_PREPARE_COMMAND)
		temp += f.Link("#prepare", "PREPARE") + ", ";
	temp += f.Link("#reveal", "REVEAL") + ", ";
	temp += f.Link("#show", "SHOW") + ", ";
	temp += f.Link("#spoils", "SPOILS") + ", ";
	if(!Globals->USE_WEAPON_ARMOR_COMMAND)
		temp += "and ";
	temp += f.Link("#tactics", "TACTICS");
	if(Globals->USE_WEAPON_ARMOR_COMMAND) {
		temp += ", and ";
		temp += f.Link("#weapon", "WEAPON");
	}
	temp += " orders are processed.";
	f.TagText("li", temp);
	temp = f.Link("#find", "FIND") + " orders are processed.";
	f.TagText("li", temp);
	temp = f.Link("#leave", "LEAVE") + " orders are processed.";
	f.TagText("li", temp);
	temp = f.Link("#enter", "ENTER") + " orders are processed.";
	f.TagText("li", temp);
	temp = f.Link("#promote", "PROMOTE") + " orders are processed.";
	f.TagText("li", temp);
	f.Enclose(0, "ul");
	f.Enclose(0, "li");
	f.Enclose(1, "li");
	f.PutStr("Combat is processed.");
	f.Enclose(1, "ul");
	temp = f.Link("#attack", "ATTACK") + " orders are processed.";
	f.TagText("li", temp);
	f.Enclose(0, "ul");
	f.Enclose(0, "li");
	if (has_stea) {
		f.Enclose(1, "li");
		f.PutStr("Steal orders.");
		f.Enclose(1, "ul");
		temp = f.Link("#steal", "STEAL") + " and ";
		temp += f.Link("#assassinate", "ASSASSINATE") +
			" orders are processed.";
		f.TagText("li", temp);
		f.Enclose(0, "ul");
		f.Enclose(0, "li");
	}
	f.Enclose(1, "li");
	f.PutStr("Give orders.");
	f.Enclose(1, "ul");
	temp = f.Link("#destroy", "DESTROY") + " and ";
	temp += f.Link("#give", "GIVE") + " orders are processed.";
	f.TagText("li", temp);
	temp = f.Link("#exchange", "EXCHANGE") + " orders are processed.";
	f.TagText("li", temp);
	f.Enclose(0, "ul");
	f.Enclose(0, "li");
	
    if(Globals->ARCADIA_MAGIC) {
    	f.Enclose(1, "li");
    	f.PutStr("Instant Magic");
    	f.Enclose(1, "ul");
    	f.TagText("li", "Old spells are cancelled.");
    	temp = "Spells are ";
    	temp += f.Link("#cast", "CAST");
    	temp += " (except for Teleportation spells).";
    	f.TagText("li", temp);
    	f.Enclose(0, "ul");
    	f.Enclose(0, "li");
    }	
	
	f.Enclose(1, "li");
	f.PutStr("Tax orders.");
	f.Enclose(1, "ul");
	temp = f.Link("#pillage","PILLAGE") + " orders are processed.";
	f.TagText("li", temp);
	temp = f.Link("#tax","TAX") + " orders are processed.";
	f.TagText("li", temp);
	f.Enclose(0, "ul");
	f.Enclose(0, "li");
	
    if(!Globals->ARCADIA_MAGIC) {
    	f.Enclose(1, "li");
    	f.PutStr("Instant Magic");
    	f.Enclose(1, "ul");
    	f.TagText("li", "Old spells are cancelled.");
    	temp = "Spells are ";
    	temp += f.Link("#cast", "CAST");
    	temp += " (except for Teleportation spells).";
    	f.TagText("li", temp);
    	f.Enclose(0, "ul");
    	f.Enclose(0, "li");
    }
	
	f.Enclose(1, "li");
	f.PutStr("Market orders.");
	f.Enclose(1, "ul");
	temp = f.Link("#guard","GUARD") + " 1 orders are processed.";
	f.TagText("li", temp);
	if(Globals->TOWNS_EXIST) {
		temp = f.Link("#sell","SELL") + " orders are processed.";
		f.TagText("li", temp);
	}
	temp = f.Link("#buy","BUY") + " orders are processed.";
	f.TagText("li", temp);
	if(Globals->SEND_COST >= 0) {
		temp = f.Link("#send","SEND") + " orders are processed.";
	    f.TagText("li", temp);
    }
	temp = f.Link("#quit","QUIT") + " and ";
	temp += f.Link("#restart", "RESTART") + " orders are processed.";
	f.TagText("li", temp);
	temp = f.Link("#forget","FORGET") + " orders are processed.";
	f.TagText("li", temp);
	f.Enclose(0, "ul");
	f.Enclose(0, "li");
	if (Globals->ALLOW_WITHDRAW) {
		f.Enclose(1, "li");
		f.PutStr("Withdraw orders.");
		f.Enclose(1, "ul");
		temp = f.Link("#withdraw","WITHDRAW") + " orders are processed.";
		f.TagText("li", temp);
		f.Enclose(0, "ul");
		f.Enclose(0, "li");
	}
	f.Enclose(1, "li");
	f.PutStr("Movement orders.");
	f.Enclose(1, "ul");
	if(may_sail) {
		temp = f.Link("#sail","SAIL") + " orders are processed.";
		f.TagText("li", temp);
	}
	temp = f.Link("#advance","ADVANCE") + " and ";
	temp += f.Link("#move", "MOVE") + " orders are processed (including any "
		"combat resulting from these orders).";
	f.TagText("li", temp);
	f.Enclose(0, "ul");
	f.Enclose(0, "li");
	f.Enclose(1, "li");
	f.PutStr("Month long orders.");
	f.Enclose(1, "ul");
	temp = f.Link("#build", "BUILD") + ", ";
	if(!(SkillDefs[S_ENTERTAINMENT].flags & SkillType::DISABLED))
		temp += f.Link("#entertain", "ENTERTAIN") + ", ";
	temp += f.Link("#produce", "PRODUCE") + ", ";
	temp += f.Link("#study", "STUDY") + ", ";
	temp += f.Link("#teach", "TEACH") + ", and ";
	temp += f.Link("#work", "WORK") + " orders are processed.";
	f.TagText("li", temp);
	temp = "Costs associated with these orders (such as study fees) are "
		"collected.";
	f.TagText("li", temp);
	f.Enclose(0, "ul");
	f.Enclose(0, "li");
	temp = "Teleportation spells are ";
	temp += f.Link("#cast", "CAST") + ".";
	f.TagText("li", temp);
	temp = "Goods from ";
	temp += f.Link("#send", "SEND") + ".";
	temp += " orders are credited.";
	f.TagText("li", temp);
	if (qm_exist) {
		temp = f.Link("#transport", "TRANSPORT") + " and " +
			f.Link("#distribute", "DISTRIBUTE") + " orders are processed.";
		f.TagText("li", temp);
	}
	f.TagText("li", "Maintenance costs are assessed.");
	f.Enclose(0, "OL");
	temp = "Where there is no other basis for deciding in which order units "
		"will be processed within a phase, units that appear higher on the "
		"report get precedence.";
	f.Paragraph(temp);
	f.LinkRef("reportformat");
	f.ClassTagText("div", "rule", "");
	f.TagText("h2", "Report Format");
	temp = "The most important sections of the turn report are the \"Events "
		"During Turn\" section which lists what happened last month, and "
		"the \"Current Status\" section which gives the description of each "
		"region in which you have units.";
	f.Paragraph(temp);
	temp = "Your units in the Current Status section are flagged with a "
		"\"*\" character. Units belonging to other factions are flagged "
		"with a \"-\" character. You may be informed which faction they "
		"belong to, if ";
	if(has_obse)
		temp += "you have high enough Observation skill or ";
	temp += "they are revealing that information.";
	f.Paragraph(temp);
	temp = "Objects are flagged with a \"+\" character.  The units listed "
		"under an object (if any) are inside the object.  The first unit "
		"listed under an object is its owner.";
	f.Paragraph(temp);
	temp = "If you can see a unit, you can see any large items it is "
		"carrying.  This means all items other than silver";
	if(!(ItemDefs[I_HERBS].flags & ItemType::DISABLED))
		temp += ", herbs,";
	temp += " and other small items (which are of zero size units, and are "
		"small enough to be easily concealed). Items carried by your own "
		"units of course will always be listed.";
	f.Paragraph(temp);
	temp = "At the bottom of your turn report is an Orders Template.  This "
		"template gives you a formatted orders form, with all of your "
		"units listed. You may use this to fill in your orders, or write "
		"them on your own. The ";
	temp += f.Link("#option", "OPTION") + " order gives you the option of "
		"giving more or less information in this template, or turning it "
		"of altogether. You can precede orders with an '@' sign in your "
		"orders, in which case they will appear in your template on the "
		"next turn's report.";
	f.Paragraph(temp);
	f.LinkRef("hintsfornew");
	f.ClassTagText("div", "rule", "");
	f.TagText("h2", "Hints for New Players");
	temp = "Make sure to use the correct #ATLANTIS and UNIT lines in your "
		"orders.";
	f.Paragraph(temp);
	temp = "Always have a month's supply of spare cash in every region in "
		"which you have units, so that even if they are deprived of "
		"income for a month (due to a mistake in your orders, for "
		"example), they will not starve to death.  It is very frustrating "
		"to have half your faction wiped out because you neglected to "
		"provide enough money for them to live on.";
	f.Paragraph(temp);
	temp = "Be conservative with your money. ";
	if(Globals->LEADERS_EXIST) {
		temp += "Leaders especially are very hard to maintain, as they "
			"cannot usually earn enough by ";
		temp += f.Link("#work", "WORK") + "ing to pay their maintenance "
			"fee. ";
	}
	temp += "Even once you have recruited men, notice that it is "
		"expensive for them to ";
	temp += f.Link("#study", "STUDY") + " (and become productive units), "
		"so be sure to save money to that end.";
	f.Paragraph(temp);
	temp = "Don't leave it until the last minute to send orders.  If "
		"there is a delay in the mailer, your orders will not arrive "
		"on time, and turns will NOT be rerun, nor will it be possible "
		"to change the data file for the benefit of players whose orders "
		"weren't there by the deadline.  If you are going to send your "
		"orders at the last minute, send a preliminary set earlier in the "
		"week so that at worst your faction will not be left with no "
		"orders at all.";
	f.Paragraph(temp);

	if(Globals->HAVE_EMAIL_SPECIAL_COMMANDS) {
		f.LinkRef("specialcommands");
		f.ClassTagText("div", "rule", "");
		f.TagText("h2", "Special Commands");
		temp = "These special commands have been added via the scripts "
			"processing the email to help you interact with the game "
			"and submit times and rumors. Please read over these new "
			"commands and their uses. Also note that all commands sent "
			"to the server are logged, including orders submissions, so "
			"if you have a problem, or if you attempt to abuse the system, "
			"it will get noticed and it will be tracked down.";
		f.Paragraph(temp);
		f.LinkRef("_create");
		f.ClassTagText("div", "rule", "");
		f.TagText("h4", "#create \"faction name\" \"password\"");
		temp = "This will create a new faction with the desired name and "
			"password, and it will use the player's \"from\" address as "
			"the email address of record (this, of course, can be changed "
			"from within the game).";
		f.Paragraph(temp);
		temp = "The \"\" characters are required. If they are missing, the "
			"server will not create the faction.";
		f.Paragraph(temp);
		f.Paragraph("Example:");
		temp = "Join the game as the faction named \"Mighty Ducks\" with the "
			"password of \"quack\"";
		temp2="#create \"Mighty Ducks\" \"quack\"";
		f.CommandExample(temp, temp2);

		f.LinkRef("_resend");
		f.ClassTagText("div", "rule", "");
		f.TagText("h4", "#resend [faction] \"password\"");
		temp = "The faction number and your current password (if you have "
			"one) are required. The most recent turn report will be sent to "
			"the address of record.";
		f.Paragraph(temp);
		f.Paragraph("Example:");
		temp = "You are faction 999 with password \"quack\" and need another "
			"copy of the last turn (because your harddrive crashed)";
		temp2 = "#resend 999 \"quack\"";
		f.CommandExample(temp, temp2);

		f.LinkRef("_times");
		f.ClassTagText("div", "rule", "");
		f.TagText("h4", "#times [faction] \"password\"");
		f.PutStr("[body of article]");
		f.TagText("h4", "#end");
		temp = "Everything between the #times and #end lines is included "
			"in your article. Your article will be marked as being "
			"sent by your fation, so you need not include that "
			"attribution in the article.";
		if (Globals->TIMES_REWARD) {
			temp += " You will receive $";
			temp += Globals->TIMES_REWARD;
			temp += " for submitting the article.";
		}
		f.Paragraph(temp);
		f.Paragraph("Example:");
		temp = "Faction 999 wants to declare war on everyone";
		temp2 = "#times 999 \"quack\"\n";
		temp2 += "The Mighty Ducks declare war on the world!!\n";
		temp2 += "Quack!\n";
		temp2 += "#end";
		f.CommandExample(temp, temp2);
		temp = "And it would appear something like:";
		temp2 = "---------------------------------\n";
		temp2 += "The Mighty Ducks declare war on the world!!\n";
		temp2 += "Quack!\n\n";
		temp2 += "[Article submitted by The Mighty Ducks (999)]\n";
		temp2 += "---------------------------------";
		f.CommandExample(temp, temp2);

		f.LinkRef("_rumor");
		f.ClassTagText("div", "rule", "");
		f.TagText("h4", "#rumor [faction] \"password\"");
		f.PutStr("[body of rumor]");
		f.TagText("h4", "#end");
		temp = "Submit a rumor for publication in the next news.  These "
			"articles are not attributed (unlike times articles) and will "
			"appear in the rumor section of the next news in a random order.";
		f.Paragraph(temp);
		f.Paragraph("Example:");
		temp = "Start a malicious rumor";
		temp2 = "#rumor 999 \"quack\"\n";
		temp2 += "Oleg is a running-dog lackey of Azthar Spleenmonger.\n";
		temp2 += "#end";
		f.CommandExample(temp, temp2);

		f.LinkRef("_remind");
		f.ClassTagText("div", "rule", "");
		f.TagText("h4", "#remind [faction] \"password\"");
		temp = "This order will have the server find the most recent set of "
			"orders you have submitted for the current turn and mail them "
			"back to your address of record.";
		f.Paragraph(temp);
		f.Paragraph("Example:");
		temp = "Remind faction 999 of its last order set.";
		temp2 = "#remind 999 \"quack\"";
		f.CommandExample(temp, temp2);

		f.LinkRef("_email");
		f.ClassTagText("div", "rule", "");
		f.TagText("h4", "#email [unit]");
		f.PutStr("[text of email]");
		temp = "This command allows you to send email to the owner of a unit "
			"even when you cannot see that unit's faction affiliation.  You "
			"will not be told who the unit belongs to, but will simply "
			"forward your email to them. When you use this command, they "
			"will recieve YOUR email and can contact you if they choose. It "
			"is provided simply as a courtesy to players to help with "
			"diplomacy in first contact situations.";
		f.Paragraph(temp);
		temp = "There is no need for a \"#end\" line (such as is used in "
			"times and rumor submissions -- the entire email message you "
			"send will be forwarded to the unit's master.";
		f.Paragraph(temp);
		f.Paragraph("Example:");
		temp = "Send an email to the owner of unit 9999";
		temp2 = "#email 9999\n";
		temp2 += "Greetings.  You've entered the Kingdom of Foo.\n";
		temp2 += "Please contact us.\n\n";
		temp2 += "Lord Foo\n";
		temp2 += "foo@some.email";
		f.CommandExample(temp, temp2);
		temp = "Faction X, the owner of 9999 would receive:";
		temp2 = "From: Foo &lt;foo@some.email&gt;\n";
		temp2 += "Subject:  Greetings!\n\n";
		temp2 += "#email 9999\n";
		temp2 += "Greetings.  You've entered the Kingdom of Foo.\n";
		temp2 += "Please contact us.\n\n";
		temp2 += "Lord Foo\n";
		temp2 += "foo@some.email";
		f.CommandExample(temp, temp2);
	}
	f.LinkRef("credits");
	f.ClassTagText("div", "rule", "");
	f.TagText("h2", "Credits");
	temp = "Atlantis was originally created and programmed by Russell "
		"Wallace. Russell Wallace created Atlantis 1.0, and partially "
		"designed Atlantis 2.0 and Atlantis 3.0.";
	f.Paragraph(temp);
	temp = "Geoff Dunbar designed and programmed Atlantis 2.0, 3.0, and 4.0 "
		"up through version 4.0.4 and created the Atlantis Project to "
		"freely release and maintain the Atlantis source code.";
	f.Paragraph(temp);
	temp = "Larry Stanbery created the Atlantis 4.0.4+ derivative.";
	f.Paragraph(temp);
	temp = f.Link("mailto:jtraub@dragoncat.net", "JT Traub");
	temp += " took over the source code and merged the then forking versions "
		"of 4.0.4c and 4.0.4+ back into 4.0.5 along with modifications of his "
		"own and has been maintaining the code.";
	f.Paragraph(temp);
	temp = "Various things have happened since and the codebase now "
        "appears to be largely run by someone's spleen.";
	f.Paragraph(temp);
	temp = "This version of the code is based on Atlantis 5.0.0 as "
        "available at August 2003 and has since been substantially "
        "modified by Bradley Steel.";
	f.Paragraph(temp);
	temp = "Acknowledgement must go to Ursala Le Guin's Earthsea books, "
	    "on which some aspects of the magic system in this game are "
	    "based - although the majority comes from the development of the "
        "Atlantis community mentioned below.";
	f.Paragraph(temp);
	temp = "The server and website on which the Arcadia games are "
        "hosted were set up by Piotr Jakubowicz.";
	f.Paragraph(temp);
	temp = "The satellite helper program, Atlantis Little Helper, "
        "was kindly modified by its developer Max Shariy to be "
        "compatible with the changes in Arcadia III.";
	f.Paragraph(temp);

	temp = "Development of the code is open and there is a egroup devoted to "
		"it located at ";
	temp += f.Link("http://groups.yahoo.com/group/atlantisdev",
			"The YahooGroups AtlantisDev egroup");
	temp += ". Please join this egroup if you work on the code and share your "
		"changes back into the codebase as a whole";
	f.Paragraph(temp);
	temp = "Please see the CREDITS file in the source distribution for a "
		"complete (hopefully) list of all contributers.";
	f.Paragraph(temp);
	
	
	//Arcadia Stuff
	//Riding defence not included because no weapon is enabled which has a riding attack.
	f.LinkRef("appendixa");
	f.ClassTagText("div", "rule", "");
	f.TagText("h2", "Appendix A");
	f.TagText("h3", "FAQ");

	f.Paragraph("");

	temp = "Q: Is there any way of displaying my report other than as text?<br></br>";
	temp += "A: Max Shariy has written a program called Atlantis Little Helper, which is "
	        "compatible with the Nylandor version of Atlantis. It is available from "
	        "http://members.shaw.ca/mshariy/ah.html. There are a number of other clients "
	        "written for Atlantis games in general, but they will not support all the "
	        "features of Nylandor. There are some minor details in ALH which are set to "
            "the default values of Atlantis games, rather than Nylandor. Namely, "
            "the cost of tactics is 50 silver in Nylandor, not 200, the weights "
            "and capacities of some monsters (including dragons and balrogs) have been "
            "changed, and pearls are no longer a trade good.";
    f.Paragraph(temp);
    
	temp = "Q: If a unit has a horse but no riding skill, can he ride?<br></br>";
	temp += "A: The riding skill only affects combat, so out of combat, he can ride. However, "
            "he will not be able to use the horse to ride in combat.";
    f.Paragraph(temp);
    
	temp = "Q: My unit has Capacity: 0/40/100/0. What do all the numbers mean? <br></br>";
	temp += "A: The first number is the total weight the unit can fly with. If the unit weighs more "
            "than this, it will not be able to fly. The second is for riding movement, and the third "
            "for walking movement. Thus, if the unit in question has a weight of 40 or less, it will "
            "ride when given the MOVE or ADVANCE order. However, if it weighs 60, it will walk, and if it weighs "
            "101 or more, it will not be able to move. The fourth number listed is 'swimming' capacity, "
            "which allows units to travel across an ocean without a boat. In almost all instances, this "
            "number will be zero.";
    f.Paragraph(temp);

	temp = "Q: If my unit has reached its maximum knowledge level, can it still study? <br></br>";
	temp += "A: Yes. Further study will provide the same experience as using the skill normally; "
            "this is usually 10, but will be less if the unit does not specialise in that skill "
            "(leaders are counted as specialising in all non-magic skills). However, it would "
            "obviously be better for the unit to get practise for real, so that you do not have "
            "to pay study costs.";
    f.Paragraph(temp);

	temp = "Q: If something doesn't make sense, where do I go for help? <br></br>";
	temp += "A: Ask on the game's yahoogroup. If you are playing, you should have already signed "
            "up, but the address is http://groups.yahoo.com/group/arcadia-pbm/ just in case.";
    f.Paragraph(temp);

	temp = "Q: Do I have to submit times articles? <br></br>";
	temp += "A: No, but it is strongly requested. The times is the main place for roleplay and "
            "interaction between players. The Arcadia series of Atlantis was begun as an attempt "
            "to get away from the 'big-game' sydnrome of having many players who do not know each "
            "other, and interact only through their units. By storytelling in the times, everyone "
            "benefits from the creation of a world with many facets.";
    f.Paragraph(temp);

	temp = "Q: My mage never has enough energy to cast spells. What should I do? <br></br>";
	temp += "A: Magic skills can be split into two sections - the foundations, and the spells. "
            "Studying spells gives you more choices as to what to cast. Studying foundations gives "
            "your mage more energy with which to cast them. The skill 'inner strength' also helps "
            "increase a mage's energy, though it is not technically a foundation skill. If you find "
            "you have more energy than you are using, study some more spells. Remember, you can "
            "cast up to three different spells per turn, plus use other spells in battle.";
    f.Paragraph(temp);

	temp = "Q: After years of studying I finally got a staff of fire. But my mage keeps casting storms instead! <br></br>";
	temp += "A: Your mage will use his 'combat' spell (set with the COMBAT order) in battle. He will only use battle items, "
            "such as a staff of fire or lightning, if he has no COMBAT spell set. To clear your combat spell, issue "
            "the order 'COMBAT' with no arguments.";
    f.Paragraph(temp);

	temp = "Q: I cast a ranged spell and specified the region it was to be cast in, but I got the error message 'No region specified'. <br></br>";
	temp += "A: Pay close attention to the spell template. If it has the word REGION, UNIT, UNITS, DIRECTION, or similar "
            "present, then those words must be used when writing the order. For instance, the spell "
            "description for farsight may say to use the format 'CAST Farsight REGION <x> <y> <z>'. With "
            "this template, one possible cast order would be 'CAST Farsight REGION 5 7 1'. The order 'CAST "
            "Farsight 5 7 1' will NOT work.";
    f.Paragraph(temp);

	temp = "Q: Do I have to use the full spell name when casting? <br></br>";
	temp += "A: No, the abbrevation will work as well. For instance, 'CAST Bird_Lore DIRECTION NE' and "
            "'CAST BIRD DIRECTION NE' do exactly the same thing. Also, for spells which require "
            "a region to be specified, not specifying a region will cause the spell to default to "
            "the mage's current location (eg 'CAST Blizzard' will cause a blizzard to be cast "
            "on the mage's current location).";
    f.Paragraph(temp);

	temp = "Q: Is there any way to reduce the amount of orders I have to write? <br></br>";
	temp += "A: Not really. The '@' and 'TURN/ENDTURN' commands are very useful though. "
            "For instance, if you want a unit to cast earthlore every turn, use the order "
            "@CAST Earth_Lore, which will cause the order to be included into your orders "
            "template for next turn. Then write your next orders based on your order template "
            "(if you are using ALH to write orders, it will do this automatically).";
    f.Paragraph(temp);
    
	temp = "Q: Can I prevent enemy ships entering an ocean region? <br></br>";
	temp += "A: No. Units which are guarding can usually prevent units from unfriendly factions "
            "entering a hex (unless they wish to initiate a battle). However, this does not work "
            "at sea, and an enemy can always sail into the hex. Of course, if you set them hostile "
            "you will attack them as soon as they do so. Ships sailing along rivers can be stopped "
            "by unfriendly guards, however the ship will move one hexside in the region being guarded "
            "and then stop, rather than being prevented from entering the region.";
    f.Paragraph(temp);
    
	temp = "Q: Why do my spells always fizzle? <br></br>";
	temp += "A: The chance of spells fizzling depends on the fraction of your mages maximum energy "
            "which comes from his mysticism skill. Thus, if you don't study mysticism, your spells "
            "will never fizzle; also, if you study more in other fundamentals, the mystic component "
            "will be smaller, and your spells will fizzle less often. Mathematically, if m "
            "is the fraction of your mage's energy which comes from mysticism, and l is your mage's "
            "skill level in mysticism, then the chance of a fizzle or backfire for a mage not specialising in mysticism is "
            "4m/(l+5). This chance is halved if your mage specialises in mysticism. As an example, "
            "a mage with patterning 3, mysticism 2, and no speciality, would have a maximum energy of 9 "
            "without his mysticism skill, and 18 with it. In this case, m = 50%, so the chance of a fizzle "
            "or backfire would be 4*50/7 = 29%. If the same mage studied mysticism to level 5, he would "
            "have 45 energy with m = 80%, but his extra skill at handling mystic forces means that his "
            "backfiring chance only increases slightly, to 4*80/10 = 32%.";
    f.Paragraph(temp);
    
	temp = "Q: A spell description says it costs 10 energy to cast on 4 men. How much will it cost to cast on one man? <br></br>";
	temp += "A: All spell costs are worked out on a per man basis, but the skill description "
	        "may not reflect this in order to give you are more accurate measure of the cost for "
	        "casting on multiple men. In this case, the cost for one man would be 10/4 = 2.5, rounded "
	        "up to a cost of 3 to cast on one man (and 5 energy to cast on two, 8 on 3, etc.). "
            "Note that if this is, say, the second spell cast, then the 50% extra cost of "
            "casting a second spell will be applied before rounding, so the cost would be 2.5*1.5 = "
            "3.75, rounded up to 4, rather than 3*1.5=4.5, rounded up to 5.";
    f.Paragraph(temp);
    
	temp = "Q: Is Nylandor bug-free? <br></br>";
	temp += "A: No. Nylandor is based on Atlantis V5, which is now close to bug-free. However, there have been many "
            "modifications made for Nylandor, and while testing has been done to reduce the number of bugs, there "
            "are certainly some remaining. If you become aware of a bug in the program (either harmful OR beneficial) please "
            "let your GM know about it, and it will be fixed as soon as possible. In serious "
            "cases the gamefile may be modified to remove the result of the bug, or the turn "
            "may be rerun. The existance of bugs (hopefully rare) is the price "
            "players pay for playing in games with dynamic rules - and, we like "
            "to believe, superior to the original.";
    f.Paragraph(temp);
    
	temp = "Q: If two mages both have fog as their combat spell, will they both cast it? <br></br>";
	temp += "A: No. There are a few combat spells which are not cumulative - these include fog, darkness, "
            "concealment, and their counter-spells. To save energy, only the best skilled mage from each army will cast "
            "these spells in combat; however if this mage runs out of energy, then the second mage "
            "will cast instead. Note though, that if the spell fizzles, the second mage will not attempt "
            "to cast during that combat round. Shield spells do not follow this pattern - multiple shields may "
            "be cast, in case the first shield is destroyed. Mages will not be sensible enough to stop casting "
            "shields even if they defend against nothing.";
    f.Paragraph(temp);

	temp = "Q: If my mage is set to cast 'dispel illusions', but there are no illusions present, what happens? <br></br>";
	temp += "A: The mage will realise that his spell will do nothing, and will not cast it. This saves his "
            "energy, but also means he is not going to get much battle experience in the spell. "
            "This applies to all offensive spells which only target certain creatures; if they cannot hit "
            "anyone, they will not get cast.";
    f.Paragraph(temp);
    
	temp = "Q: I am having trouble with TEACH and BUILD orders for units on ships. What's going on? <br></br>";
	temp += "A: Unit aliases (such as 'NEW 1') are cleared whenever a unit moves from its original hex. "
            "Because SAIL takes place before TEACH orders, this means that if a 'NEW' unit sails, any "
            "attempt to teach it (such as TEACH NEW 1) will fail. This only applies to newly formed units, "
            "and unfortunately there is not yet a way around it. Additionally, although BUILD orders are "
            "processed after SAIL orders, any unit which is on a moving ship and attempts to BUILD will "
            "fail his order. This is a side-effect of the way in which BUILD orders are processed, and cannot be avoided."
            "If you intended the structure to be built in the region the ship is leaving, then you should "
            "give the unit a LEAVE or ENTER order in addition to the BUILD order. If you wanted to BUILD "
            "a structure in the region the ship is sailing to, you must wait until the next month, when your "
            "builders can disembark and BUILD a structure.";
    f.Paragraph(temp);    
    
   	temp = "Q: What riding skill do monsters have? <br></br>";
	temp += "A: When trying to catch a monster to initiate a battle, the monster will have an effective riding "
            "skill of 4 if it flies, 2 if it rides, or 0 if it can only walk or swim. However, when the monster "
            "is attempting to initiate a battle, it will have an effective riding skill of 6, 3 or 0 in these cases.";
    f.Paragraph(temp);    

   	temp = "Q: Do roads increase wages in regions where they are built. <br></br>";
	temp += "A: This is a common feature in many Atlantis games, but is not enabled "
            "in Nylandor.";
    f.Paragraph(temp);
    
   	temp = "Q: The spell description for some summoned creatures says they have a maintenance cost which decreases at "
           " higher skill levels. How do I know how much it will be? <br></br>";
	temp += "A: The cost quoted is accurate at skill level 1. Every level above this reduces the cost by 10%, so the "
            "maintenance will be 90% of the quoted value at level 2, down to 50% at level 6. Note that if the maintenance "
            "cost is fractional, it will be rounded up, but only after any other effects are taken into account.";
    f.Paragraph(temp);

	temp = "Q: What determines the rate at which a settlement grows? <br></br>";
	temp += "A: Settlements (villages, towns and cities) will stay at their initial population until someone "
            "produces, buys or sells in the region. Producing in the region can change the population "
            "slightly; this occurs equally in all regions regardless of the presence of a settlement. "
            "Buying and selling items can make a big difference to the population of the settlement. "
            "Generally, there is a mimimum amount that must be traded before the settlement will grow "
            "at all, and every additional bit will help it to grow more. But there is also a maximum rate "
            "at which a settlement will grow, so there is no point selling more than two to three times "
            "the minimum amount needed. For all growth calculations, selling a food item has double the "
            "effect of selling a non-food item (such as stone), and trade items count triple when bought "
            "or sold. If trade in the settlement stops, the population will gradually "
            "decline over time, but will never fall below 80% of the biggest size the settlement reached. "
            "Also, settlements which demand few foodstuffs (grain, livestock and fish) are easier to grow than "
            "a settlement of the same size that demands more foodstuffs.";
    f.Paragraph(temp);
    
   	temp = "Q: What is the exact equation for growth of a settlement? <br></br>";
	temp += "A: Let T be the total demand for food items, multiplied by 2 (the bonus factor for food items), "
            "plus the total demand and availability of trade items, multiplied by 3. "
            "Let t be the total trade occuring in the settlement, given by the sum of all normal trades, "
            "plus 2 times all food trades, plus 3 times all trades of 'trade items'. Let P be the population "
            "of the settlement (this is slightly less than the total population of the region). Define M as "
            "'P/6000 - 0.08'. If t < MT, the settlement will lose population equal to (P-Pt/MT)/20, but will "
            "not drop below 80% of the greatest size the settlement has previously reached. If t > MT, the "
            "city will grow by G*(1- exp(1-(t/MT)) ), where G is '150 + P(6000-P)/60000'.";
    f.Paragraph(temp);

   	temp = "Q: How many casualties will my mage's spell XXXX cause in a battle? <br></br>";
	temp += "Spell damages are generated randomly, so will always be different. However, "
            "we can work out some averages for a particular spell. Summon tornado is quoted "
            "as getting 2-100*lvl chances-to-attack. That is, at level 2 it gets 2-200 "
            "chances-to-attack, or 101 on average. In most terrains, each chance to attack "
            "has a 50% success rate, so he will on average make 50.5 attacks. In forests "
            "or jungles, ranged and magic attacks suffer a -1 penalty, so the mage will "
            "make 101/3 = 33.7 attacks on average. <br></br> " 
            "Each attack is a 'weather' attack made with skill 2. Men in the open defend "
            "against magic attacks with skill 0, ie at a penalty of -2 relative to the "
            "casting mage. Each attack thus has a 4:1, ie 80% chance of killing a man "
            "in the open. Thus, this spell will, on average, kill about 40 men in most "
            "terrains. In forests or jungles, the mage suffers a -1 skill penalty, so "
            "only gets a 2:1, or 67% success rate for his 33.7 attacks, and will thus "
            "kill about 22 men. <br></br> "
            "Some creatures have a higher defence against weather attacks; for instance "
            "skeletons have a defence of 3. Against an all-skeleton army, this mage "
            "would (in normal terrain) have a 1:2 success rate, ie 33%, and would kill"
            "about 50.5/3 ~ 17 skeletons. Men in forts or other buildings get a +2 bonus "
            "to magic defence (more if it has runewords), as well as the caster getting "
            "a -1 penalty to chance-to-attack. So if the men in the previous example "
            "were in a fort, the mage would attack for 33% (-1 penalty) of his "
            "chances-to-attack, with a 50% (0 skill difference) kill rate per attack, "
            "thus killing 101*0.33*0.5 ~ 17 men on average (down from 40). If these "
            "fortified men were in a jungle (do this one yourself) this comes down "
            "to 5.05 kills, on average.";
    f.Paragraph(temp);
    
   	temp = "Q: How are new dragons generated? <br></br>";
	temp += "A: You take a mummy and a daddy, and next month you have a baby ... <br></br> "
            "Well, actually, dragons are flexible: you only need two dragons in a hex to produce "
            "a baby, there is no worry about what sex they are. Three or four dragons will also "
            "produce only one baby, but five dragons will produce two; eight dragons will produce "
            "three, eleven dragons will produce four, etc. And if you have eleven dragons in your "
            "territory producing babies, then you shouldn't be reading this. You should be running "
            "for your life.";
    f.Paragraph(temp);

   	temp = "Q: When do baby dragons become dragons? <br></br>";
	temp += "A: Twelve months after they are 'born', a baby dragon will turn into an adult "
            "dragon, ready to terrorise your peasants and breed more babies. Baby dragons "
            "cannot swim, so are less of a threat than adult dragons if there is ocean "
            "between you and them. And don't forget - "
            "away from Bashkeil, if two wandering dragons meet up, they are likely to decide "
            "to wander together and merge into a single unit. Best to kill dragons while they're "
            "on their own, else things can get nasty.";
    f.Paragraph(temp); 

	f.LinkRef("appendixb");
	f.ClassTagText("div", "rule", "");
	f.TagText("h2", "Appendix B");
	f.TagText("h3", "Monster Table");

	f.Paragraph("");
	
	f.Enclose(1, "center");
	f.Enclose(1, "table border=\"1\"");
	f.Enclose(1, "tr");
	f.TagText("th", "Monster");
	f.TagText("th", "Movement");
	f.TagText("th", "Attacks");
	f.TagText("th", "Hits");
	f.TagText("th", "Attack Skill");
	f.TagText("th", "Defence Skill*");
	f.TagText("th", "Magic Skill");
	f.TagText("th", "Spoils**");
	f.TagText("th", "Possible Spoils");

	f.Enclose(0, "tr");

	for(i = 0; i < NITEMS; i++) {
		if(ItemDefs[i].flags & ItemType::DISABLED) continue;
		if(!(ItemDefs[i].type & IT_MONSTER)) continue;
		if(ItemDefs[i].type & IT_ILLUSION) continue;
		MonType *mp = FindMonster(ItemDefs[i].abr, 0);

		f.Enclose(1, "tr");
		f.Enclose(1, "td align=\"left\" nowrap");
		f.PutStr(ItemDefs[i].name);
		f.Enclose(0, "td");
		f.Enclose(1, "td align=\"left\" nowrap");
		if(ItemDefs[i].fly) f.PutStr("Flying");
		else if(ItemDefs[i].ride) f.PutStr("Riding");
		else f.PutStr("Foot");
		f.Enclose(0, "td");
		f.Enclose(1, "td align=\"left\" nowrap");
		f.PutStr(mp->numAttacks);
		f.Enclose(0, "td");
		f.Enclose(1, "td align=\"left\" nowrap");
		f.PutStr(mp->hits);
		f.Enclose(0, "td");
		f.Enclose(1, "td align=\"left\" nowrap");
		f.PutStr(mp->attackLevel);
		f.Enclose(0, "td");
		f.Enclose(1, "td align=\"left\" nowrap");
		f.PutStr(AString(mp->defense[ATTACK_COMBAT])+AString(mp->defense[ATTACK_RANGED]) + " " +
		                 AString(mp->defense[ATTACK_ENERGY])+AString(mp->defense[ATTACK_SPIRIT])+AString(mp->defense[ATTACK_WEATHER]));
		f.Enclose(0, "td");
		f.Enclose(1, "td align=\"left\" nowrap");
		if(mp->special == NULL) f.PutStr("&nbsp;");
		else f.PutStr("Yes");
		f.Enclose(0, "td");
		f.Enclose(1, "td align=\"left\" nowrap");
		f.PutStr(mp->silver);
		f.Enclose(0, "td");
		f.Enclose(1, "td align=\"left\" nowrap");
		switch(mp->spoiltype) {
		    case -1:
		        f.PutStr("Silver");
		        break;
            case IT_NORMAL:
                //hardcoded values, could be generalised. Applies to BS modded spoils code.
                if(mp->silver < 20) f.PutStr("Silver");
                else if(mp->silver < 30) f.PutStr("Silver and food items");
                else if(mp->silver < 60) f.PutStr("Silver and primary normal items");
                else f.PutStr("Silver, normal and trade items");
                break;
		    case IT_ADVANCED:
		        if(mp->silver < 100) f.PutStr("Silver");
		        else if(mp->silver < 200) f.PutStr("Silver, IRWD, FLOA, MITH, ROOT, MUSH");
		        else if(mp->silver < 400) f.PutStr("Silver and advanced items except DBOW, MARM");
		        else if(mp->silver < 500) f.PutStr("Silver and advanced items except MARM");
		        else f.PutStr("Silver and advanced items");
		        break;
            case IT_MAGIC:
            default:
                temp = "Silver";
                for(int j=0; j<NITEMS; j++) {
                    if(ItemDefs[j].flags & ItemType::DISABLED) continue;
                    if(!(ItemDefs[j].type & mp->spoiltype)) continue;
                    if(ItemDefs[j].type & IT_SPECIAL) continue;
                    if(ItemDefs[j].baseprice > mp->silver) continue;
                    temp += ", ";
                    temp += ItemDefs[j].abr;
                }
                f.PutStr(temp);
                break;
		}
		f.Enclose(0, "td");
		f.Enclose(0, "tr");
	}
	f.Enclose(0, "table");
	f.Enclose(0, "center");
	
	temp = "*   The defence skill lists five numbers, against (in order) melee attacks, "
             "ranged attacks, energy attacks, spirit attacks, and weather attacks.";
	f.Paragraph(temp);
	temp = "**  Spoils are randomly generated. On average, you will get half the amount listed in "
             "silver, and an amount of goods valued, by the game engine, at equal to this "
             "amount (if the monster is capable of dropping goods). However, being randomly "
             "generated, you may get more or less spoils than expected.";
	f.Paragraph(temp);
	
	f.LinkRef("appendixc");
	f.ClassTagText("div", "rule", "");
	f.TagText("h2", "Appendix C");
	f.TagText("h3", "Item Table");

	f.Paragraph("");
	
	f.Enclose(1, "center");
	f.Enclose(1, "table border=\"1\"");
	f.Enclose(1, "tr");
	f.TagText("th", "Abbr");
	f.TagText("th", "Item");
	f.TagText("th", "Weight (capacity)");
	f.TagText("th", "Type");
	f.TagText("th", "Skill");
	f.TagText("th", "Level");
	f.TagText("th", "Material");
	f.TagText("th", "Months");
	f.TagText("th", "Magic Skill");
	f.TagText("th", "Level");
	f.TagText("th", "Material");
	f.TagText("th", "Sold by");
	f.Enclose(0, "tr");
	SkillType *mS;
	int numtrade = 0;
	
	for(i = 0; i < NITEMS; i++) {
		if(ItemDefs[i].flags & ItemType::DISABLED) continue;
		if((ItemDefs[i].type & IT_MAN) || (ItemDefs[i].type & IT_MONSTER)) continue;
		if(ItemDefs[i].type & IT_SPECIAL) continue;
		pS = FindSkill(ItemDefs[i].pSkill);
		mS = FindSkill(ItemDefs[i].mSkill);

		f.Enclose(1, "tr");
		f.Enclose(1, "td align=\"left\" nowrap");
		f.PutStr(ItemDefs[i].abr);
		f.Enclose(0, "td");
		f.Enclose(1, "td align=\"left\" nowrap");
		f.PutStr(ItemDefs[i].name);
		f.Enclose(0, "td");
		f.Enclose(1, "td align=\"left\" nowrap");
		temp = ItemDefs[i].weight;
		cap = ItemDefs[i].walk - ItemDefs[i].weight;
		if(ItemDefs[i].walk || (ItemDefs[i].hitchItem != -1)) {
			temp += " (";
			if(ItemDefs[i].hitchItem == -1)
				temp += cap;
			else {
				temp += (cap + ItemDefs[i].hitchwalk);
				temp += " with ";
				temp += ItemDefs[ItemDefs[i].hitchItem].name;
			}
			temp += ")";
		}
		f.PutStr(temp);
		f.Enclose(0, "td");
		f.Enclose(1, "td align=\"left\" nowrap");
		temp = " ";
		int any = 0;
		if(ItemDefs[i].type & IT_NORMAL) {
		    temp += "normal";
		    any = 1;
		}
		if(ItemDefs[i].type & IT_ADVANCED) {
		    if(any) temp += ", ";
		    else any = 1;
		    temp += "advanced";
		}
		if(ItemDefs[i].type & IT_MAGIC) {
		    if(any) temp += ", ";
		    else any = 1;
		    temp += "magic";
		}
		if(ItemDefs[i].type & IT_WEAPON) {
		    if(any) temp += ", ";
		    else any = 1;
		    temp += "weapon";
		}
		if(ItemDefs[i].type & IT_ARMOR) {
		    if(any) temp += ", ";
		    else any = 1;
		    temp += "armour";
		}
		if(ItemDefs[i].type & IT_MOUNT) {
		    if(any) temp += ", ";
		    else any = 1;
		    temp += "mount";
		}
		if(ItemDefs[i].type & IT_TRADE) {
		    if(any) temp += ", ";
		    else any = 1;
		    temp += "trade";
		}
		if(ItemDefs[i].type & IT_TOOL) {
		    if(any) temp += ", ";
		    else any = 1;
		    temp += "tool";
		}		
		if(ItemDefs[i].type & IT_FOOD) {
		    if(any) temp += ", ";
		    else any = 1;
		    temp += "food";
		}
		f.PutStr(temp);
		f.Enclose(0, "td");
		f.Enclose(1, "td align=\"left\" nowrap");
		if(pS != NULL) f.PutStr(pS->name);
		else f.PutStr("&nbsp;");
		f.Enclose(0, "td");
		f.Enclose(1, "td align=\"left\" nowrap");
		if(pS != NULL) f.PutStr(ItemDefs[i].pLevel);
		else f.PutStr("&nbsp;");
		f.Enclose(0, "td");		
		f.Enclose(1, "td align=\"left\" nowrap");
		comma = 0;
		temp = "";
		if (ItemDefs[i].flags & ItemType::ORINPUTS)
			temp = "Any of : ";
		for(j = 0; j < (int) (sizeof(ItemDefs->pInput) /
						sizeof(ItemDefs->pInput[0])); j++) {
			k = ItemDefs[i].pInput[j].item;
			if(k < 0 || (ItemDefs[k].flags&ItemType::DISABLED))
				continue;
			if(comma) temp += ", ";
			temp += ItemDefs[i].pInput[j].amt;
			temp += " ";
			if(ItemDefs[i].pInput[j].amt > 1)
				temp += ItemDefs[k].names;
			else
				temp += ItemDefs[k].name;
			comma = 1;
		}
		if(temp == "") temp = "&nbsp;";
		f.PutStr(temp);
		f.Enclose(0, "td");
		f.Enclose(1, "td align=\"left\" nowrap");
		if(ItemDefs[i].pMonths) {
			temp = ItemDefs[i].pMonths;
		} else {
			temp = "&nbsp;";
		}
		f.PutStr(temp);
		f.Enclose(0, "td");
		f.Enclose(1, "td align=\"left\" nowrap");
		if(mS != NULL) f.PutStr(mS->name);
		else f.PutStr("&nbsp;");
		f.Enclose(0, "td");
		f.Enclose(1, "td align=\"left\" nowrap");
		if(mS != NULL) f.PutStr(ItemDefs[i].mLevel);
		else f.PutStr("&nbsp;");
		f.Enclose(0, "td");		
		f.Enclose(1, "td align=\"left\" nowrap");
		comma = 0;
		temp = "";
		if (ItemDefs[i].flags & ItemType::ORINPUTS)
			temp = "Any of : ";
		for(j = 0; j < (int) (sizeof(ItemDefs->mInput) /
						sizeof(ItemDefs->mInput[0])); j++) {
			k = ItemDefs[i].mInput[j].item;
			if(k < 0 || (ItemDefs[k].flags&ItemType::DISABLED))
				continue;
			if(comma) temp += ", ";
			temp += ItemDefs[i].mInput[j].amt;
			temp += " ";
			if(ItemDefs[i].mInput[j].amt > 1)
				temp += ItemDefs[k].names;
			else
				temp += ItemDefs[k].name;
			comma = 1;
		}
		if(temp == "") temp = "&nbsp;";
		f.PutStr(temp);
		f.Enclose(0, "td");
		
		f.Enclose(1, "td align=\"left\" nowrap");
		if(ItemDefs[i].type & IT_TRADE) {
		    if(numtrade < 3) f.PutStr("Durhemmen");
		    else if(numtrade < 6) f.PutStr("Usskil");
		    else if(numtrade < 9) f.PutStr("Enwas");
		    else if(numtrade < 12) f.PutStr("Vemish");
		    else if(numtrade == 12) f.PutStr("Durhemmen, Vemish");
		    else if(numtrade == 13) f.PutStr("Durhemmen, Usskil");
		    else if(numtrade == 14) f.PutStr("Usskil, Enwas");
		    else if(numtrade == 15) f.PutStr("Enwas, Vemish");
		    else if(numtrade == 16) f.PutStr("Usskil, Vemish");
		    else if(numtrade == 17) f.PutStr("Durhemmen, Enwas");
		    else if(numtrade == 18) f.PutStr("&nbsp;");
		    numtrade++;
		} else f.PutStr("&nbsp;");
		f.Enclose(0, "td");	

		f.Enclose(0, "tr");
	}
	f.Enclose(0, "table");
	f.Enclose(0, "center");


	f.LinkRef("appendixd");
	f.ClassTagText("div", "rule", "");
	f.TagText("h2", "Appendix D");
	f.TagText("h3", "Weapon Table");

	f.Paragraph("");
	
	f.Enclose(1, "center");
	f.Enclose(1, "table border=\"1\"");
	f.Enclose(1, "tr");
	f.TagText("th", "Weapon");
	f.TagText("th", "Skill Required*");
	f.TagText("th", "Attack Type");
	f.TagText("th", "Attack Bonus");
	f.TagText("th", "Armour Piercing");
	f.TagText("th", "Num Attacks");

	f.Enclose(0, "tr");
	int asterix = 0;
	for(i = 0; i < NITEMS; i++) {
	    if(ItemDefs[i].flags & ItemType::DISABLED) continue;
		if(!(ItemDefs[i].type & IT_WEAPON)) continue;
		
		WeaponType *pW = FindWeapon(ItemDefs[i].abr);

		f.Enclose(1, "tr");
		f.Enclose(1, "td align=\"left\" nowrap");
		f.PutStr(ItemDefs[i].name);
		f.Enclose(0, "td");
		f.Enclose(1, "td align=\"center\" nowrap");
		if(pW->flags & WeaponType::NEEDSKILL) {
		    pS = FindSkill(pW->baseSkill);
		    temp = pS->name;
		    pS = FindSkill(pW->orSkill);
			if(pS) temp += AString(" or ") + pS->name;
		} else temp = "&nbsp;";
		f.PutStr(temp);
		f.Enclose(0, "td");
		f.Enclose(1, "td align=\"center\" nowrap");
		f.PutStr(AttType(pW->attackType));
		f.Enclose(0, "td");
		f.Enclose(1, "td align=\"center\" nowrap");
		if(pW->mountBonus) f.PutStr(AString(pW->attackBonus+pW->mountBonus/2) + "**");
		else f.PutStr(pW->attackBonus);
		f.Enclose(0, "td");
		f.Enclose(1, "td align=\"center\" nowrap");
		if(pW->weapClass == ARMORPIERCING) f.PutStr("Yes");
		else f.PutStr("&nbsp;");
		f.Enclose(0, "td");
		f.Enclose(1, "td align=\"center\" nowrap");
		if(pW->numAttacks == WeaponType::NUM_ATTACKS_HALF_SKILL) {
            f.PutStr("half skill level***");
            asterix = 1;
        }
		else if(pW->numAttacks == WeaponType::NUM_ATTACKS_SKILL) f.PutStr("skill level");
		else if(pW->numAttacks > 0) f.PutStr(AString(pW->numAttacks) + " per round");
		else f.PutStr(AString("1 every ") + AString(-(pW->numAttacks)) + " rounds");
		f.Enclose(0, "td");
		f.Enclose(0, "tr");
	}
	f.Enclose(0, "table");
	f.Enclose(0, "center");
	
	temp = "*   If no skill is required, then the weapon bonus is added onto the unit's combat "
             "skill (if any) and the unit's riding skill, if they have an appropriate mount and "
             "are in appropriate terrain. If the weapon requires a longbow or crossbow skill, then "
             "the wielder defends with a skill of zero. Otherwise the wielder defends with his "
             "attack skill.";
	f.Paragraph(temp);
	/*
	temp = "**  This weapon provides a bonus of 4 to defence, and an average bonus of 4 to attack. However, the wielder's attack skill "
             "is modified by whether or not a soldier he attacks is riding; if he is, then this weapon "
             "gets a bonus of 2 (attack bonus = 6), if the target is not riding (or is a monster), then "
             "there is a penalty of 2 (attack bonus = 2).";
	f.Paragraph(temp);
	*/
    if(asterix) {
        temp = "***  Number of attacks is equal to the unit's skill level, divided by 2, and rounded up.";
        f.Paragraph(temp);
    }             
	
	
	
	f.LinkRef("appendixe");
	f.ClassTagText("div", "rule", "");
	f.TagText("h2", "Appendix E");
	f.TagText("h3", "Armour Table");

	f.Paragraph("");
	temp = "Armour has a fixed chance to block enemy attacks against the wearer. The percentage for each "
        "armour is given in the item description, rounded to the nearest percent. This table lists instead "
        "the 'survivability' for each armour - that is, how many attacks are needed, on average, for one attack to "
        "pass through the armour and kill the wearer (ie a '4' in the table means that there is a 1 in 4 chance "
        "of an attack killing the wearer). Note that this is an average - some soldiers may survive "
        "more hits than shown here, and others may die with the first hit.";
	f.Paragraph(temp);
	
	f.Enclose(1, "center");
	f.Enclose(1, "table border=\"1\"");
	f.Enclose(1, "tr");
	f.TagText("th", "Armour");
	f.TagText("th", "Normal");
	f.TagText("th", "Armour-Piercing");
	f.TagText("th", "Magical");

	f.Enclose(0, "tr");
	for(i = 0; i < NITEMS; i++) {
	    if(ItemDefs[i].flags & ItemType::DISABLED) continue;
		if(!(ItemDefs[i].type & IT_ARMOR)) continue;
		ArmorType *at = FindArmor(ItemDefs[i].abr);

		f.Enclose(1, "tr");
		f.Enclose(1, "td align=\"left\" nowrap");
		f.PutStr(ItemDefs[i].name);
		f.Enclose(0, "td");
		f.Enclose(1, "td align=\"center\" nowrap");
		if(at->from == at->saves[PIERCING]) f.PutStr("infinite");
		else {
		    int ones = at->from / (at->from - at->saves[PIERCING]);
		    int tenths = (10*at->from) / (at->from - at->saves[PIERCING]);
		    tenths -= 10*ones;
		    temp = AString(ones);
		    if(tenths) temp += AString(".") + tenths;
		}
		f.PutStr(temp);
		f.Enclose(0, "td");
		f.Enclose(1, "td align=\"center\" nowrap");
		if(at->from == at->saves[ARMORPIERCING]) f.PutStr("infinite");
		else {
		    int ones = at->from / (at->from - at->saves[ARMORPIERCING]);
		    int tenths = (10*at->from) / (at->from - at->saves[ARMORPIERCING]);
		    tenths -= 10*ones;
		    temp = AString(ones);
		    if(tenths) temp += AString(".") + tenths;
		}
		f.PutStr(temp);
		f.Enclose(0, "td");
		f.Enclose(1, "td align=\"center\" nowrap");
		if(at->from == at->saves[MAGIC_ENERGY]) f.PutStr("infinite");
		else {
		    int ones = at->from / (at->from - at->saves[MAGIC_ENERGY]);
		    int tenths = (10*at->from) / (at->from - at->saves[MAGIC_ENERGY]);
		    tenths -= 10*ones;
		    temp = AString(ones);
		    if(tenths) temp += AString(".") + tenths;
		}
		f.PutStr(temp);
		f.Enclose(0, "td");
		f.Enclose(0, "tr");
	}
	f.Enclose(0, "table");
	f.Enclose(0, "center");
	
	f.LinkRef("appendixf");
	f.ClassTagText("div", "rule", "");
	f.TagText("h2", "Appendix F");
	f.TagText("h3", "Terrain Table");

	temp = "Properties of individual regions are generated randomly, according to "
	    "the parameters in the table below. Note that the population and wages "
	    "of a region will increase if a settlement (village, town or city) "
        "is present. The population of the region and of the settlement are "
        "calculated separately, but you will only ever see the combined population "
        "on your report. A settlement with less than 1000 people is called a "
        "village and increases wages by 1; a settlement with 1000-1999 people "
        "is called a town and increases wages by 2, and a settlement with "
        "2000+ people is called a city and increases wages by 3. The population "
        "of a region may be increased by up to 50% of its initial value due to "
        "production in the region; the population of a settlement may be "
        "increased through trading goods in the market.";
	f.Paragraph(temp);


	f.Enclose(1, "center");
	f.Enclose(1, "table border=\"1\"");
	f.Enclose(1, "tr");
	f.TagText("th", "Terrain");
	f.TagText("th", "Move Cost*");
	f.TagText("th", "Population");
	f.TagText("th", "Wages");
	f.TagText("th", "Grain/Livestock");
	f.TagText("th", "Normal Resources**");
	f.TagText("th", "Advanced Resources**");


	f.Enclose(0, "tr");
	for(i = 0; i < R_NUM; i++) {
	    if(TerrainDefs[i].flags & TerrainType::DISABLED) continue;
	    if(i == R_NEXUS) continue;

		f.Enclose(1, "tr");
		f.Enclose(1, "td align=\"left\" nowrap");
		f.PutStr(TerrainDefs[i].name);
		f.Enclose(0, "td");
		f.Enclose(1, "td align=\"left\" nowrap");
		f.PutStr(TerrainDefs[i].movepoints);
		f.Enclose(0, "td");
		f.Enclose(1, "td align=\"left\" nowrap");
		temp = AString(TerrainDefs[i].pop/2) + "-" + (TerrainDefs[i].pop-1);
		if(TerrainDefs[i].pop == 0) temp = AString('-');
		f.PutStr(temp);
		f.Enclose(0, "td");
		f.Enclose(1, "td align=\"left\" nowrap");
		temp = AString(TerrainDefs[i].wages) + "-" + (TerrainDefs[i].wages+2);
		f.PutStr(temp);
		f.Enclose(0, "td");
		f.Enclose(1, "td align=\"left\" nowrap");
		temp = AString(TerrainDefs[i].economy) + "-" + (2*TerrainDefs[i].economy-1);
		if(TerrainDefs[i].economy == 0) temp = AString('-');
		f.PutStr(temp);
		f.Enclose(0, "td");
		f.Enclose(1, "td align=\"left\" nowrap");
		temp = "";
		first = 1;
		for(unsigned int c = 0; c < sizeof(TerrainDefs[i].prods)/sizeof(Product); c++) {
		    if(TerrainDefs[i].prods[c].product > -1 && (ItemDefs[TerrainDefs[i].prods[c].product].type & IT_NORMAL)) {
		        if(first) first = 0;
		        else {
		            temp += ", ";
		        }
		        temp += AString(TerrainDefs[i].prods[c].amount) + "-" + (2*TerrainDefs[i].prods[c].amount-1);
		        temp += AString(" ") + ItemDefs[TerrainDefs[i].prods[c].product].names;
		        if(TerrainDefs[i].prods[c].chance != 100) temp += AString(" (") + TerrainDefs[i].prods[c].chance + "%)";
		    }
		}
		if(first) temp = "&nbsp;";
		f.PutStr(temp);
		f.Enclose(0, "td");
		f.Enclose(1, "td align=\"left\" nowrap");
		temp = "";
		first = 1;
		for(unsigned int c = 0; c < sizeof(TerrainDefs[i].prods)/sizeof(Product); c++) {
		    if(TerrainDefs[i].prods[c].product > -1 && !(ItemDefs[TerrainDefs[i].prods[c].product].type & IT_NORMAL)) {
		        if(first) first = 0;
		        else {
		            temp += ", ";
		        }
		        temp += AString(TerrainDefs[i].prods[c].amount) + "-" + (2*TerrainDefs[i].prods[c].amount-1);
		        temp += AString(" ") + ItemDefs[TerrainDefs[i].prods[c].product].names;
		        if(TerrainDefs[i].prods[c].chance != 100) temp += AString(" (") + TerrainDefs[i].prods[c].chance + "%)";
		    }
		}
		if(first) temp = "&nbsp;";
		f.PutStr(temp);
		f.Enclose(0, "td");
		f.Enclose(0, "tr");
	}
	f.Enclose(0, "table");
	f.Enclose(0, "center");

	temp = "*   This is the move cost for units walking, riding, or (for ocean and lakes) "
             "swimming. Flying units have a cost of 1 to enter any terrain type. All units "
             "have their movement cost doubled in bad weather (winter, monsoons). Units may "
             "not enter a region during a blizzard. Sailing ships to the centre of any ("
             "lake or ocean) region costs 2 movepoints in bad weather or 5 in a blizzard, "
             "while sailing to edge terrain costs 1 in bad weather or 3 in a blizzard. Sailing "
             "in good weather always costs one movement point.";
	f.Paragraph(temp);

	temp = "**  If a percentage is listed after a resource, then that resource is not "
             "always present in that region type. The percentage listed is the approximate "
             "chance of finding that resource in that terrain type.";
	f.Paragraph(temp);
	
	f.LinkRef("appendixg");
	f.ClassTagText("div", "rule", "");
	f.TagText("h2", "Appendix G");
	f.TagText("h3", "Guard Table");


	f.Enclose(1, "center");
	f.Enclose(1, "table border=\"1\"");
	f.Enclose(1, "tr");
	f.TagText("th", "Settlement");
	f.TagText("th", "Guards");
	f.TagText("th", "Armour");
	f.TagText("th", "Skills");
	f.TagText("th", "Items");

	f.Enclose(0, "tr");
	for(i = 0; i < 7; i++) {
		f.Enclose(1, "tr");
		f.Enclose(1, "td align=\"left\" nowrap");
		switch(i) {
		    case 0: f.PutStr("Village");
		        break;
		    case 1: f.PutStr("Town");
		        break;
		    case 2: f.PutStr("City");
		        break;
		    case 3: f.PutStr("City (West)");
		        break;
		    case 4: f.PutStr("City (North)");
		        break;
		    case 5: f.PutStr("City (East)");
		        break;
		    case 6: f.PutStr("City (South)");
		        break;
        }		        
		f.Enclose(0, "td");
		f.Enclose(1, "td align=\"left\" nowrap");
		if(i<3) f.PutStr(AString(20*(i+1)));
		else f.PutStr(AString(1));
		f.Enclose(0, "td");
		f.Enclose(1, "td align=\"left\" nowrap");
		if(i<2) f.PutStr("none");
		else f.PutStr("leather");
		f.Enclose(0, "td");
		f.Enclose(1, "td align=\"left\" nowrap");
		if(i < 3) {
    		temp = AString("Combat ") + (i+1) + ", Observation " + (i+2);
    		if(i) temp += AString(", Tactics ") + i;
    		f.PutStr(temp);
		} else {
		    switch(i) {
		        case 3:
		            f.PutStr("Combat 3, Tactics 2, Fire 3");
		            break;
                case 4:
                    f.PutStr("Combat 3, Tactics 2");
                    break;
                case 5:
                    f.PutStr("Combat 3, Tactics 2, Instill Courage 3");
                    break;
                case 6:
                    f.PutStr("Combat 3, Tactics 2, Aura of Fear 2");
                    break;
            }
		}
		f.Enclose(0, "td");
		f.Enclose(1, "td align=\"left\" nowrap");
        temp = "&nbsp;";
        if(i == 4) {
            temp = "32 wolves, 4 eagles";
        } else if(i == 5) {
            temp = "100 illusory imps";
        }
		f.PutStr(temp);
		f.Enclose(0, "td");
		f.Enclose(0, "tr");
	}
	f.Enclose(0, "table");
	f.Enclose(0, "center");
}
