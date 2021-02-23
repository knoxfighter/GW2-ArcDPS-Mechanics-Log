# [Click Here to Download](/../../releases)

# Overview
MarsEdge has been working on a plugin for ArcDPS using its realtime combat api. The plugin shows a realtime list of failed mechanics for players. The mechanics are determined by the skill id of the attack a player/npc is hit by. It shows a timestamp in ‘min:sec’ since entering combat and ‘X was hit by Y mechanic’.

This plugin is not intended to breed toxicity, but instead help show players mechanical areas where the players can improve. This is in the same way that arcdps shows how dps/boons could be improved.

Here are some example screenshots (with names redacted):

![log](https://i.imgur.com/yHWWstN.jpg)

![chart](https://i.imgur.com/dcP57iR.png)

[(I use a custom arcdps font found here)](https://mega.nz/#!ck5kVL7I!T1Llgtnz5H6jRCnHWcCa0D5kJoqgE_0rudvscyDUT8I)
# Currently detected mechanics
<details><summary>Show list</summary>
<p>

- Vale Guardian
  - teleport
- Gorseval
  - slam
  - egg
  - touching an orb
- Sabetha
  - time bombs
  - sapper bombs
  - standing in cannon fire
- Slothasor
  - tantrum
  - bomb (holding)
  - standing in bomb AoE
  - flame breath
  - shake
  - fixate
- Bandit Trio
  - throwing bees
  - throwing oils
- Matthias
  - reflecting shards (that grant Matthias a damage buff)
  - corruption (to fountain)
  - bomb (to wall)
  - sacrifice
  - touching a ghost
  - fire tornado
  - storm cloud
- Keep Construct
  - fixate
  - touching the core
- Xera
  - triggering orb explosion
  - standing in orb AoE (after orb explosion)
  - magic
  - teleport
- Cairn
  - teleport
  - missing a green circle
  - slap
- Samarog
  - shockwave
  - slap
  - bludgeon (vertical smack during last 33%)
  - fixate
- Deimos
  - oil
  - smash
  - picking up a tear
  - teleport (having the green circle)
- Soulless Horror
  - standing in the inner/outer ring (the attack avoided for Necro Dancer)
  - standing in torment aoe
  - standing in pie slices
  - touching a scythe
  - taking fixation
  - gaining the tanking damage debuff
  - CCing a Tormented Dead
- Soul Eater
  - puke
  - spider webs
  - picking up an orb
  - throwing an orb
- Broken King
  - getting a green
- Eye Cave
  - CCing the eyes
- Dhuum
  - touching a messenger
  - getting affliction
  - affliction explosion damage
  - shackles
  - cracks
  - dip aoe
  - touching center during suction
  - cone
  - snatch (on CM)
- Conjured Amalgamate
  - squash (arm slamming down on player)
  - using swords/shields
- Twin Largos
  - shockwave
  - waterlogged stacks
  - bubble
  - getting a tidal pool
  - standing in whirlpool
  - geyser
  - boon steal
  - dash
- Qadim
  - shockwave
  - arcing fire lines
  - teleport
  - being inside the hitbox
- Adina (Earth Djinn)
  - was blinded
  - looked at eye
  - touched pillar ripple
  - touched a mine
- Sabir (Air Djinn)
  - big tornado
  - shockwave
  - bubble
  - bop at phase
- Qadim the Peerless
  - is tank
  - touched lava
  - expanding lightning
  - purple rectangle
- Fractals of the Mists
  - flux bombs
  - Nightmare
    - MAMA
      - flaming whirl
      - cone knockback
      - giant leap
      - standing in acid
      - knight’s hammer smash
    - Siax
        - standing in acid
    - Ensolyss
        - lunge (being run over)
        - smash attack with the 5 adds
  - Shattered Observatory
    - Arkk
      - standing in pie slices
      - cone knockback
      - getting a bomb
      - getting a green circle
- Strike Missions
  - Boneskinner
    - Charge
    - Death Wind
  - Fraenir
    - Icequake
    - Ice Shock Wave
    - Freeze
  - Kodan Brothers
    - Trap
  - Whisper of Jormag
    - Chains
    - Hit by Chains
    - Preading ice
    - Icy Slice
    - Tornado
  - Icebrood Construct
    - Shock Wave
    - Deadly Shockwave
    - Arm Swing
    - Ice Crystal
    - Flail
    - Ice Shatter
  
</p>
</details>

If you have any ideas for things I’m missing, please let me know, preferably through github.

I have no current interest in mechanics detection for PvP / WvW. I’m not even sure what kind of things would be useful to show from those game modes.

# Installation:

This requires a recent version of arcdps . When in doubt, get the latest from here.

Put the .dll file in the same folder as arcdps (Program Files/Guild Wars 2/bin64 by default).

To disable, just remove the .dll or move it to a different folder.

This plugin shouldn’t need to be updated with Guild Wars 2 updates, but arcdps must be compatible with Guild Wars 2. It does need to be updated for new encounters / abilities.

If arcdps updates the combat api, this plugin might need to be updated for compatibility.

# Usage:

Don’t be a dick.

Open the arcdps options panel (Alt+Shift+T by default) and enable the “Mechanics Log” or “Mechanics Chart” checkboxes at the bottom

or use the hotkeys Alt+Shift+L (Log) / Alt+Shift+N (Chart)

# Troubleshooting:

Be sure you’re running the latest version of arcdps.

Ensure directx9 june2010 runtime is installed (needed for arcdps and this plugin)

Ensure visual c 2013 x64 redist is installed (needed for arcdps and this plugin)

Ensure the .dll file is in the “bin64″ folder, not the “bin” folder

Right click on the .dll file -> go to properties. If there is an “Unblock” button, click that button and apply changes

If options are not being saved, manually create a file named “arcdps_mechanics.ini” next to the arcdps.ini (Default file location is “C:\Program Files\Guild Wars 2\addons\arcdps\arcdps_mechanics.ini”)

# Notes:

If an attack is successfully blocked, evaded, or invulned, then it is not listed.

Attacks that interrupt (Gorseval slams, Samarog slaps, etc) are not listed if the player has stability.

The combat api has a small delay (~2 seconds) before the mechanic will be shown.

All the combat data used by this plugin are in the arcdps .evtc log files anyway. This simply parses it in realtime.

Some of the mechanics are not necessarily bad. Getting a bomb on sloth is normal, but if the bomb is then dropped in a bad place, you can see who had it.

Vapor Form / Illusion of Life do not count as 2nd downs in the chart

# Reading the Chart:

A “neutral” mechanic is a mechanic that a player might have to deal with during the fight. This isn’t bad, but it might lower that player’s personal DPS to do the mechanic (e.g. Matthias corruption, Dhuum shackles, etc)

A “failed” mechanic is a negative mechanic that a player was hit by (e.g. Vale Guardian teleports, expanding a Deimos oil, etc)

Some mechanics are not necessarily the hit player’s fault. If 1 sub-group gets slammed on Gorseval, that might be the chrono’s fault (depending on the group’s strategy).

# FAQ:

Q: Is it Terms of Service compliant?

A: Yes. MarsEdge has talked to Chris Cleary from Anet and he did not see any issues with compliance. ([part1](https://i.imgur.com/hwAe5L9.png) [part2](https://i.imgur.com/pnLhxQd.png))

# Known Issues:

If a player has a magnetic aura, reflecting distortion, etc and reflects a shard on Matthias, it will say the player with the buff reflected and not the player that granted the buff.

The chart will track downs/deaths of players in open-world PvE / WvW squads. This can get spammy. For now, I recommend clearing the chart before starting instanced content to avoid the clutter.

Send bug reports, feedback, hate mail, etc through github.
# Special thanks to:
- deltaconnected for arcdps and the combat api
- [KA] Karma and Omg Its Kayla.6479 for testing, feedback, and encouragement
- MarsEdge for creating this tool

# Original Changelog:
<details><summary>Show changelog</summary>
<p>

    July 9 2019
        Fix Arkk green/bomb mechanics not being tracked if player is downed
        Add tracking for:
            Dhuum affliction explosion damage
            Sabir bop at phase (chart only by default to avoid spam)
                Special thanks to [REZ] Snackrificial Pugs for inspiring the addition of this mechanic
            Qadim2
                pylon lava (forgot a 3rd id. oops)
                expanding lightning
                purple rectangle
    June 25 2019
        Add tracking for:
            Sabir (Air Djinn)
                big tornado
                shockwave
                bubble
            Qadim the Peerless
                touching small lava(missed a skill id before)
            Special thanks to the Elite Insights devs for finding the skill ids
        Fix extra separator at top of log (makes copy to clipboard smoother)
    June 21 2019
        Add initial wing 7 mechanics (more to come soon tm)
            Adina (Earth Djinn)
                was blinded (chart only by default, quite spammy)
                looked at eye
                touched pillar ripple
                touched a mine
            Qadim the Peerless
                is tank
                touched lava
        Add some additional checks for compatibility with other plugin configurations
    June 10 2019
        Redo chart UI
        Consistify capitalization of filter options
        Make mechanic options detailed description [?] only show if a description exists (none yet)
        Don’t show multiple placeholders in a row in the log
        Add account name to chart
        Use arc exports instead of reading the ini
        Add tracking for:
            Bandit Trio
                throwing bees
                throwing oils
            Matthias
                touching a ghost
                fire tornado
                storm cloud
        Add option to export csv on close
        Add dynamic version string instead of being hardcoded 0.3 forever
    May 15 2019
        Fix some squad members not being tracked when joining an instance with 9 others already inside
    May 11 2019
        Expand log/chart filters to include separate fields for the player, mechanic, boss
        Add tracking for CCing an eye on w5 statues
        Use Ender’s Echo buff instead of damage ticks for tracking Dhuum CM snatches
            This fixes false positive when coming back from a green into the echo
            Special thanks to Aikan for finding this
    April 23 2019
        Add tracking for CCing a Tormented Dead on Soulless Horror
            This tracks stun (including Tides of Time variant) and daze. No other CC will be tracked. This will not catch everything, but the meta for SH is such that this will catch most accidental CCs while ignoring intentional knockback skills
    March 25 2019
        Show boss name for each mechanic in chart
        Add mechanic description plumbing (not active yet)
        Add tracking for
            Using CA sword/shields
                Special thanks to Google.1364 and CombatEvolve.9238 for inspiring the addition of this mechanic
            Touching the KC core
            Green circle tracking for Arkk
            getting a green on ice king
        Fix Soul Eater orbs being considered failed mechanics
        Attempt to put separators before mechanics that happen on enter combat (mainly impacts fixates as entering combat)
            Special thanks to Izzy.2093 for inspiring this addition
        Misc code cleanup
    February 1 2019
        Add tracking for throwing an orb on Soul Eater
            Special thanks to Brokenyew.3901 for inspiring the addition of this mechanic
        Add filter text box to the chart
            (more filter options coming soon)
        Don’t show players in chart who aren’t currently in squad and only have downs/deaths
        Hide UI during loading screens / at character select
    January 1 2019
        Make chart export button open the csv
        Change menus to mimic arcdps’s new style
    December 18 2018
        Add tracking for being inside Qadim’s hitbox
        Add additional checks for empty player data
    December 4 2018
        Remove some non-working mechanics from the options panel
        Rename several mechanics for more clarity
            Sloth
                stood in a bomb aoe -> stood in bomb aoe
            Xera
                touched an orb -> triggered an orb
            Samarog
                was hit by shockwave -> was shockwaved
            Deimos
                was hit by smash -> was smashed
            Soulless Horror
                stood in inner/outer donut -> stood in inner/outer ring
            Dhuum
                stood in a mark -> stood in a poison mark
                touched the center -> was sucked center
                stood in a teleport aoe -> stood in dip aoe
            MAMA
                was hit by wirl -> was hit by whirl
            Feedback on these or any other wording changes is welcome. One of my goals has been to be sure that when someone looks at the log they can quickly understand what they were hit by.
        Add check to be sure the chart csv export path isn’t empty
        Add additional checks for if a player id is 0
            (fixes a rare case where lagging players might get missing/malformed combat data)
            Special thanks to [TK] Toxic Knights for helping me track this one down
        Add tracking for Qadim’s teleport
        Significant performance optimizations
            went from ~3% CPU usage to <1%
        Fix a mechanic rarely appearing twice in the log
        Fix touching a gorseval orb showing up in the log more times than intended
        Misc code cleanup
    October 7 2018
        Add UI panel to choose where mechanics are displayed (log vs chart vs hidden)
        Fix wing 6 bosses not having a name in the .csv output
        Add option to only show mechanics affecting yourself
        Add mechanics tracking for
            Twin Largos
                Boon steal
                geyser
                dash
        Fix Qadim shockwave detection only tracking waves from the destroyer and not Qadim himself
    October 2 2018
        This version fixes compatibility with arcdps versions from October 2 2018 and newer. Using old versons of this mechanics plugin with new versions of arcdps will yield completely inaccurate results
        Update for arcdps event structure changes
        Change “ate shockwave” to “was shockwaved” in log
    October 1 2018
        Add tracking for some wing 6 mechanics
            Coagulated Amalgamate
                squashed (arm slamming down in player)
            Twin Largos
                shockwave
                waterlogged stacks (chart only)
                bubble
                getting a tidal pool
                standing in whirlpool
            Qadim
                shockwave
                arcing fire lines (chart only)
        Fix flux bombs being counted as a “failed” mechanic
        Add experimental ini support for choosing which mechanics are in the log vs chart vs hidden (UI for this coming soon)
            0 = hidden
            1 = chart only
            2 = log only
            3 = both
        Change window names to title case to match the experimental arcdps naming conventions
    September 11 2018
        Add tracking for
            WARNING: These require arcdps from September 10 2018 or newer. This is the ‘experimental’ arcdps build at the time of this posting. Using an older arcdps build will work, but the below mechanics will be missing from the output
            When someone GETS the Xera magic, instead of when the magic is used
            Xera teleports
            Deimos teleports
            FotM flux bombs
            Dhuum putrid marks (chart only)
        Fix some mechanics not being detected if a player gets multiple mechanics in a very short time frame
        Fix closing a Deimos tear being counted as a ‘Fail’
        Fix cairn slaps being improperly detected when a player has stability
        Improved Deimos oil tracking
            This should be more robust now. It’s not gonna be 100% accurate for extremely chaotic situations, but it should be easy to know who’s responsible from the output
        Rename “Received” mechanics to “Neutral” mechanics
    July 16 2018
        Remove KC east/west fixate tracking
            Unfortunately, the KC changes make the east and west sides now use the same id. As such, they are no longer distinguishable. 🙁
    July 10 2018
        Fix mysterious issue where some computers couldn’t load the .dll file
    July 10 2018
        MAJOR rewrite of almost all of the internal tracking
            Breakage is possible everywhere
        Add detection for:
            Dhuum cone
            Cairn slap
                Special thanks to Mike the Gemini for inspiring the addition of this mechanic
            Separate detection for KC east vs west fixate
                Special thanks to [Meta] Dungeon Ducks for inspiring the addition of this mechanic
                This was working before KC was broken. This might no longer be accurate/possible. I’ll fix this once KC is confirmed to be stable
                Hopefully they still have separate ids for the different sides
        Use the Arc ini settings for click settings (game restart required on change)
        Fix a very rare crash when too many players are hit by mechanics within 1 game tick
        Lay groundwork for arcdps cbtevent1 change (not active yet)
        Don’t export the csv if there’s nothing in it
    April 4 2018
        Add detection for:
            Soul eater puke
            Soul eater spider web
        Fix hotkeys not unlocking when GW2 is not the active window
        Code cleanup
    April 2 2018
        Remove April Fools changes in case someone takes them seriously
    March 31 2018
        Fix the mechanics log filter field from sometimes stealing input when pressing TAB
    March 28 2018
        Fix hotkeys passing input to the game when pressed
    March 26 2018
        Add detection for:
            Picking up demonic tears
            Deimos smash during last 10%
        Add (hopefully) better Deimos oil tracking
            This should not list as many people after the initial person expands the oil. Let me know of any issues / inaccuracies. I haven’t had any deviation from dps.report in my testing
        Disable Dhuum dying on green detection
            This should have been disabled for the last release since it doesn’t work. Oops.
        Fix Xera magic listing the person with magic multiple times
        Add hotkey support for the mechanics log/chart
            By default:
                Alt+Shift+L = Log
                Alt+Shift+N = Chart
            Can be changed in config as decimal (not hex) key codes from here
        Add some additional crash safeguards
        Only show some mechanics in the log to reduce spam
            All are still shown in the chart
            Soon there will be an option per mechanic of what to display where
        Misc code cleanup
        Very minor performance optimizations
    March 3 2018
        This now requires arcdps from feb.27.2018 or newer
        Code cleanup
        Performance optimizations (especially in boon-heavy environments)
        Fix chart breaking after clearing it
        Add Soulless Horror debuff tracking
        Make deimos smash still count if the player has stability (since stability doesn’t prevent the knockback)
        Mechanics Export
            Add player name to the csv for the mechanics lines (makes excel parsing easier)
            Add account name
            Started work for counting the number of boss pulls (currently always 0, will be added soon)
        Add support for loading/saving settings
            arcdps_mechanics.ini is next to the arcdps.ini file
            Currently saves:
                which panels were visible
                the export location for the mechanic chart exporting
        Always show the people in the same party/squad/instance in the chart
    February 8 2018
        Rename SH fixate to say the person took fixation
        Rename KC ‘fixate’ -> ‘fixated’ for consistency
        Add tracking for more green circle patterns on Cairn
        Add gorseval orb detection
        Shorten Dhuum suction aoe text
        Add initial chart exporting as text or csv
            Chart is exported to ‘My Documents\Guild Wars 2\addons\arcdps\arcdps.mechanics’
            There is currently not an option for changing the file path (soon tm)
            You can see an example export here
            Chart is auto-exported when the game is closed
        Use combat notifies to populate player list
        Fix potential underflow with vapor form manual case
    January 24 2018
        Fix Dhuum CM snatch detection not working when a player is dropped and picked up again in the same attempt
    January 21 2018
        Fix MD5 Generation
        Considerable rewright of mechanic definitions (breakage possible)
        Minor performance optimizations
        Add experimental fixation detection for sloth,souless horror,KC,sam
        Add player merging to the chart (for when a player swaps characters)
        Fix dhuum affliction not registering while in downstate
        Added separate “Clear” button to the chart
        Remove some empty space to the right of the log filter text
    January 14 2018
        Add Mechanics Chart
            Shows all mechanics recieved/failed over all pulls until manually cleared
        Add detection for:
            Dhuum – teleport aoes
            Dhuum – touching center during suction
            Dhuum CM – snatch
            Matthias – corruption
            Matthias – sacrifice
            FotM – Shattered Observatory (normal mode) – Arkk pie slice detection
        Fix some mechanics counting if hit while in downstate
        Fix FotM – Shattered Observatory CM – Arkk – bomb detection (skill id changed after the bugfixes to the fractal)
        Make standing in red xera aoes have 1 second (from 2 seconds) frequency for more granularity
        Rename the Dhuum “golem” to a “messenger”
        Rename the Soulless Horror “golem” to “torment”
        Make gui windows non-collapsible (removed the weird triangle that didn’t do anything anyways)
        Add manual case for Vapor Form / Illusion of Life not counting as a down
        Performance optimizations
    December 7 2017
        Fix Dhuum affliction occasionally showing the player hit by the bomb instead of the player with the bomb
        Add detection for Dhuum shackles
        Add detection for Dhuum cracks
    December 3 2017
        Remove erroneous Cairn shard reflect logging with new versions of arcdps
        Only show who was afflicted of Dhuum, not who was hit by it (attempt 1)
    December 2 2017
        Add mechanics detection for various Nightmare fotm mechanics
            MAMA
                Flaming wirl
                cone knockback
                giant leap
                standing in acid
                knight’s hammer smash
            Siax
                standing in acid
            Ensolyss
                lunge (being run over)
                smash attack with the 5 adds
        Added mechanics detection for some of Shattered Observatory fotm mechanics
            These skill ids might have changed with some of the various bug fixes to the fractal. If any of these mechanics are not being correctly detected, let me know.
            Arkk
                standing in pie slices
                cone knockback
                exploding a bomb outside a dome
        Added mechanics detection for some raid wing 5 mechanics
            These mechanics are not as fully tested as normal. I wanted to get a release for everyone to try out and hopefully help find any issues with the detection. I believe these all work in some capacity, but for now there might be some errors.
            Horror
                standing in the inner/outer donut
                standing in golem aoe
                standing in pie slices
                touching a scythe
            Dhuum
                touching a golem
                hitting an ally with affliction
    November 4 2017
        Add detection for standing in Sabetha cannon fire
        Add detection for missing green circles on Cairn (ignored when player has stability)
        Add MD5 hash
    October 26 2017
        Add Samarog “Bludgeon” detection (Vertical smack during last 33%)
            Special thanks to shootingstar.7160 and [BLCC] (Black Lion Cookie Company) for inspiring the addition of this mechanic
        Re-enable Xera magic detection
            Not fully tested, but it should work now
        Fix potential issue where closing and re-opening the mechanics log would prevent future logging
        Hopefully fix multiple prints for Deimos oils
            It should be good, but let me know of any found inaccuracies
        Use “=” instead of “-” to separate new boss attempts to allow for better/easier filtering
    October 22 2017
        Added gui integration with arcdps
            Enable the mechanics log with the last checkbox in the arcdps options panel (Alt+Shift+T by default)
            The mechanics log can be moved/resized
            Text can be filtered by player name or mechanic
        Partial fix for deimos oil print spaming (the 1st person is the one who activated the oil)
    October 11 2017
        Use new get_init_addr from arcdps combat api (this now requires arcdps from oct.05.2017 or newer)
        Only show 1 person touching a Deimos oil
        Add “————–” between boss attempts
        Show timestamps as min:sec as time since starting combat
        Added Sabetha sapper bomb, time bomb detection (this will currently be a bit spammy at Bandit Trio and during trash clearing)
    October 4 2017
        Initial release
</p>
</details>
