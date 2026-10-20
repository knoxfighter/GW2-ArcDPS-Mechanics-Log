# [Click Here to Download](/../../releases)

# Overview
I’ve been working on a plugin for ArcDPS using its realtime combat api. The plugin shows a realtime list of failed mechanics for players. The mechanics are determined by the skill id of the attack a player/npc is hit by. It shows a timestamp in ‘min:sec’ since entering combat and ‘X was hit by Y mechanic’.

This plugin is not intended to breed toxicity, but instead help show players mechanical areas where the players can improve. This is in the same way that arcdps shows how dps/boons could be improved.

Here are some example screenshots (with names redacted):

![log](https://i.imgur.com/yHWWstN.jpg)

![chart](https://i.imgur.com/dcP57iR.png)

# Known Issues:
- ~~Clearing the mechanics chart breaks the chart https://github.com/MerlinAesalon/GW2-ArcDPS-Mechanics-Log/issues/4~~ Clearing Chart reset stats now, doesnt remove players not in squad.
- Sunqua Peak Fractal mechanics are not complete, some are still missing.
- Striked through mechanics do not work in current version to be fixed in future.

Since i updated to a new imgui version, some graphic glitches could occur (overlapping text/elements also count). Please report them, so i can fix them.

# Mechanics Logged
### Raids
- Vale Guardian
  - teleport
- Gorseval
  - slam
  - egg
  - touching an orb
- Sabetha
  - ~~time bombs~~
  - sapper bombs
  - standing in cannon fire
- Slothasor
  - tantrum
  - bomb (holding)
  - standing in bomb AoE
  - flame breath
  - shake
  - ~~fixate~~
- Bandit Trio
  - throwing bees
  - throwing oils
- Matthias
  - reflecting shards (that grant Matthias a damage buff)
  - ~~corruption (to fountain)~~
  - ~~bomb (to wall)~~
  - ~~sacrifice~~
  - touching a ghost
  - fire tornado
  - storm cloud
- Keep Construct
  - ~~fixate~~
  - touching the core
- Xera
  - triggering orb explosion
  - standing in orb AoE (after orb explosion)
  - ~~magic~~
  - ~~teleport~~
- Cairn
  - teleport
  - missing a green circle
  - slap
- Samarog
  - shockwave
  - slap
  - bludgeon (vertical smack during last 33%)
  - ~~fixate~~
- Deimos
  - oil
  - smash
  - ~~picking up a tear~~
  - ~~teleport (having the green circle)~~
- Soulless Horror
  - standing in the inner/outer ring (the attack avoided for Necro Dancer)
  - standing in torment aoe
  - standing in pie slices
  - touching a scythe
  - taking fixation
  - ~~gaining the tanking damage debuff~~
  - ~~CCing a Tormented Dead~~
- Soul Eater
  - puke
  - spider webs
  - ~~picking up an orb~~
  - throwing an orb
- Broken King
  - getting a green
- Eye Cave
  - ~~CCing the eyes~~
- Dhuum
  - touching a messenger
  - ~~getting affliction~~
  - affliction explosion damage
  - shackles
  - cracks
  - dip aoe
  - touching center during suction
  - cone
  - ~~snatch (on CM)~~
- Conjured Amalgamate
  - squash (arm slamming down on player)
  - using swords/shields
- Twin Largos
  - shockwave
  - ~~waterlogged stacks~~
  - ~~bubble~~
  - ~~getting a tidal pool~~
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
  - ~~was blinded~~
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
- Greer, the Blightbringer
  - wave of corruption
  - blob of blight hit and ~~targeted~~.
  - ripples of rot (jump)
  - cage of decay (arrows)
- Decima, the Stormsinger
  - double hit from fluxance
  - double hit from chorus of thunder
  - seismic crash (jump knockback)
- Ura, the Steamshrieker
  - sharding
  - hit by sulfuric geysire wave
  - hit by eruption vent wave

### Fractals of the Mists
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
- Sunqua Peak
  - Ai, Keeper of the Peak
    - Red Circle 
    - Windspheres/Fireballs/Whirlpools/Laser (summoned during dashes)
    - Wind blades (intersect center of arena during split)
    - Call Meteor 
    - Firestorm
      
### Strike Missions
- Mai Trin
  - cone attack
  - side cone attack
  - shockwave attack
  - stood in puddle
  - yellow circle
  - ~~exposed buff~~
  - ~~green target~~
  - ~~green debuff~~
  - ~~green~~
  - ~~bomb~~
- Ankka
  - hands Aoe
  - pull Aoe
  - sections Aoe
  - Kraits
  - Quaggans
- Minister Li
  - knock back wave
  - burst
  - rush
  - bomb
  - ~~green target~~
  - flames
  - red circles overlapped hit
  - ~~fixated~~
  - bladestorm
  - big laser
  - ~~debilitated stack~~
  - ~~infirmity stack~~
  - ~~exposed stack~~
- Harvest Temple
  - ~~void debuff~~
  - jormag breath
  - primordus slam
  - crystal barrage
  - kralkatorrik beam
  - modremoth shockwave
  - zhaitan scream
  - whirlpool
  - soo-won tsunami
  - soo-won claw
  - revealed
- Cerus
  - hit by wall
  - hit by double wall
  - orb collection
- Dagda
  - target soul feast
  - ~~target numbers~~
  - demonic blast
  - green target
- Kela
  - sclading wave
  - tornado cc
  - ~~biting swarm~~
  - lightning strike cc
  - tackle
- Vloxx
  - Cosmic Charge
  - Annihilating Orb  
  - Annihilating Orb Shockwave without stability
  - Slice Through Reality
  - Thousand Strikes
  - Worldpiercer
  - Echoing Blade
  - Excision Extremis
  - Raging Storm
  - Float up by Visions of Eternity (Only with no stability)
  - Probability Distribution
  - Division Eternal
  - Judgement of Eternity
  - Surrounding Curse
  - Eternal Reflection
  - Ascension's Sacrifice
- Icebrood Construct
  - Ice Arm Swing
  - Ice Shatter (Ice Orbs)
  - Ice Crystal
  - Ice Flail
  - Ice Shock Wave
  - Deadly Ice Shockwave (knockdown)
  - Shatter Arm (knockdown)
- Voice and Claw
  - Hammer Slam (knockdown)
  - Hammer Spin (launch)
- Fraenir of Jormag
  - Icequake (knockdown)
  - Frozen Missile (launch)
  - Torrent of Ice (freeze)
  - Ice Shock Wave/Seismic Crush (knockdown)
  - Ice Flail (knockdown)
  - Shatter Arm (knockdown)   
- Boneskinner
  - Grasp (AoE at player's feet)
  - Charge (knockdown)
  - Death Wind (launch)
- Whisper of Jormag
  - Chains of Frost (target and hits)
  - Spreading Ice
  - Icy Slice
  - Icy Tempest (tornado)
  - Lethal Coalescence
- Cold War
  - Lethal Coalescence
  - Call Assassins
  - Flame Wall
  - Charge! (knockdown)
  - Detonate


If you have any ideas for things I’m missing, please let me know.

I have no current interest in mechanics detection for PvP / WvW. I’m not even sure what kind of things would be useful to show from those game modes. I don’t PvP / WvW much myself.

# Installation:

This requires a recent version of arcdps . When in doubt, get the latest from [here](https://www.deltaconnected.com/arcdps/).

Put the .dll file in the same folder as arcdps (Program Files/Guild Wars 2/bin64 by default).

To disable, just remove the .dll or move it to a different folder.

This plugin shouldn’t need to be updated with Guild Wars 2 updates, but arcdps must be compatible with Guild Wars 2.

If arcdps updates the combat api, this plugin might need to be updated for compatibility.

# Usage:

Don’t be a dick.

Open the arcdps options panel (Alt+Shift+T by default) and enable the “Mechanics Log” or “Mechanics Chart” checkboxes at the bottom

or use the hotkeys Alt+Shift+L (Log) / Alt+Shift+N (Chart)

# Troubleshooting:

Be sure you’re running the latest version of arcdps.

Ensure directx9 june2010 runtime is installed (needed for arcdps and this plugin)

Ensure visual c 2015-2019 x64 redist is installed (needed for arcdps and this plugin). [Microsoft Download Page](https://support.microsoft.com/en-gb/help/2977003/the-latest-supported-visual-c-downloads)

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

A: Yes. I’ve talked to Chris Cleary from Anet and he did not see any issues with compliance. ([part1](https://i.imgur.com/hwAe5L9.png) [part2](https://i.imgur.com/pnLhxQd.png))

# Known Issues:

### General
- The chart will track downs/deaths of players in open-world PvE / WvW squads. This can get spammy. For now, I recommend clearing the chart before starting instanced content to avoid the clutter.

### Mathias
- If a player has a magnetic aura, reflecting distortion, etc and reflects a shard on Matthias, it will say the player with the buff reflected and not the player that granted the buff.

Send bug reports, feedback, hate mail, etc in a new issue here or join the [Elite Insights discord](https://discord.gg/dCDEPXx) and write in the channel #mechanics-log.

# Special thanks to:
- deltaconnected for arcdps and the combat api
- Linco for updating
- [KA] Karma and Omg Its Kayla.6479 for testing, feedback, and encouragement
