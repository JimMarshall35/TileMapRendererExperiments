# Time Travelling Farming Game / JRPG

Based on Doomsday Book by Connie Willis. Set in Roughly the same fictional universe.


The year is 2100, and the future is a Utopian one. The overall tone of the story is something like an an anime.
Humans have invented time travel, and it is strictly regulated and used for academic research.

The Setting is the univeristy of manchester, and the history department is conducting a research program on farming throughout the ages.
The first destination is the 14th century. The player must choose between three characters to play as:

## Samantha
	- An earnest researcher, a bookish swat who is completely dedicated to the task of preparing for the time period she will travel back to.
	- Lawful good alignment
	- Not respected by the faculty as she's too boring, but a certain professor has complete faith in her
	- Basically the character from Doomsday book

## Gunnar
	- An obnoxious icelandic youtuber who has come up with a deal with the cash strapped university who reluctangly agree to letting him travel back in time 
	- A "pay to play" time traveller who's father donates to the uni to allow him to travel back
	- No interest in history just wants views 
	- Willing to troll time period natives for a laugh
	- Chaotic neutral alignment

## Joanna
	- A mysterious professor transferred from another faculty... alledgedly
	- Seems young, doesn't seem to care about history, seen lounging in office
	- Faculty suspicious of her, think she is an agent of the state of some kind - she is
	- True neutral alignmen
	- she works for secret governement organisation

The deal is the chosen character is on a mission. Go back to the early 14th century for a year, ostensibly for academic research.
They will have an implanted translator that will let them speak and understand the language but it is imperfect. Your cover story is that you are a dutch peasant who has fled a tyrannical lord. The idea is that this will explain your lack of knowledge about the culture to the other peaseants. Other researchers have laid the groundwork with the local lord already who belives the same thing and is sympathetic to your plight. He is told you are a skilled farmer and artisan.

## Each characters motives:

- Samantha
	- she actually does want to earnestly research 13th century farming
- Gunnar
	- he wants do get good low brow content for his youtube channel which he will film on location in the past
- Joanna 
	- she is on a secret mission to win favour with the lord and tell him about the upcoming bubonic plague and how they can avoid impact

## The game is a stardew valley type game, with elements of a JRPG. Episodic, with different time periods.

- Initially medieval
- other settings considered:
	- Victorian
	- Bronze age (stone henge era)
	- Colonial settler era USA

## It has the following unique features:
	- The framing narative described above.
	- There is an initial one year time limit in the past. Which takes maybe twice as long as a stardew valley year
	- Each character has a different series of story missions to complete 
	- Once they complete the story mision series they go back and the story advances, to a new time period. Then the player can go back at will to the 14th century and it is a normal stardew valley clone

## The flow of the game is:
	1.) Choose Character
	2.) initial cut scenes, bits where you walk around the uni, speak to people like a jrpg
	3.) go back to past
	4.) build your farm and do character specific missions
	5.) after a year go back, see a different cut scene depending on how well you did with the story missions
	6.) progress the story or go back to your farm - the uni is now a hub

The gameplay back in the past is that you must be a good farmer, you plant crops, raise animals, craft things, stay alive. There is more of a survival element to the game, you MUST feed yourself and treat diseases or you'll die.

##  There are the following mini games:
	- farming crops
	- looking after animals
	- Fishing
	- hunting / poaching
	- mining
	- foraging
	- combat - there is an area where there is an invading army of some kind, or you can go to france or something
		- also wolves and bears in wilderness, and bandits
	- speaking to people in the town, you can get into relationships with them, but if you do you'll be severely lectured when you get back to the future
	
## There are the following townspeople:
	- blacksmith
	- rival farmers and their daughters and wives
	- priest
	- jolly abbot
	- inn keeper
	- italian merchant
	- town drunk
	- snooty princess
	- local rogue
	- alchemist
	= the lord
	- his knights
	- fellow time traveller, who's an advisor to the lord - a senior professor

## Samanthas Missions:
	- Get good at farming
	- Get to know the community, do missions for them, learn their way of life
	- Gain favour with farmers, interview them
	- regular cut scenes of her writing reports, and when certain things happen

## Gunnars Missions:
	- All revolve around getting content
	- Realises he needs to improve his farm for content or he'll look bad. Sets about creating a farm that LOOKS good
	- Doing inane and morally dubious things for content, brewing strong ale for the town drunk, seducing tavern wenches, growing weed seeds he's smuggled back, showing peasants his iphone
	- mining gold so he can donate lavishly to the church mr beast style

## Joannas Missions:
	- She needs to get the attention of powerful people, so she must convince the local lord she was a big deal
	- She can pick what she says to the lord, is she:
		- a good cook
			- hunting and fishing focused mission
		- a good craftsman
			- mining and gold smithing mission
		- a good farmer
			- farming mission - produce a certain volume of certain different crops
		- a good warrior
			- combat focused
	- Once she has convinced the lord (or thinks she has) she overhears him laughing with his retainer - she's just a girl what does she know, and returns back dejected 


## When they go back to the future, each character has several possible "endings" to this chapter:

Samantha:
	- If she has failed, she faces a humiliating dressing down by the head of the history department, during which the prof who believed in her when nobody else did must stick up for her even though she DID do badly
	- If she did well a cut scene where she walks down the hallway and all her colleagues are asking her stuff, showing she's gained their respect

Gunnar:
	- If he's done well, he looks at his videos before he's even uploaded the new one and sees that they've all got WAY more views. He's subtly changed society to be more like him with his stupid antics in the past. He uploads the video and it gets a good reception - the first to crack 1 billion views. He releases a new energy drink.
	After his sudden disappearance and his lavish donation to the churc he's been cannonised as a mr beast saint. 
	- If he fails, he looks at the videos and realises he's got less. The people in he past just found him annoying and his video sucks

Joanna:s
	- If she did well, the lord did listen to her advice after all and was influential in convincing other lords prepare for the plague. She has a gadget of some kind which tells her 10,000 extra medieval texts have been preserverd in the new timeline
	- if she fails, the dean complains and uses his political connections to expose the program. She is harrassed by reporters and must face a government tribunal to explain her actions, violating the temporal prime directive.

# Technology Required

## UI

The current XML and lua based UI system will be further developed:
	- paragraph text formatting
	- perfected scrolling areas
	- animations
	- transparency

Specific UI elements needed:
	- Initial character select screen
	- Dialog screen
		- With options for what to say, dialog trees
		- Portrait and name of character spoken to
	- Pause screen
		- Tabs
			- Inventory
			- Crafting
			- Skills + Stats
			- Quest Journal
			- Map
			- Settings
	- HUD
		- Time and date
		- Item bar
		- Health and energy
		- Money
		- Items picked up appear briefly
	- Shop dialog
	- Chest inventory dialog
	- Fishing UI

## Game Framework improvements

- A mechanism for communcating between layers needs to be created. Communcation will happen by passing messages via a queue. Its needed particulary for UI layers which needs to interact with the game. In the case of the HUD, recieving periodic messages from it and in the case of the pause menu, effecting changes within the game as well. A message bus with layers listening for certain messages and sending messages onto the bus, unaware if anything is listening. Lua wrapped of course for XMLUILayer layers to be completely lua and xml defined and not coupled to a specific game implementation.

- A mechanism for "stashing" a layer would be good. For example, it would be good to keep the HUD active but not display it or have it as the top layer, and instead push the pause menu onto the top. When the pause menu closes, the HUD is then pushed back onto the active stack from the stashed one. At no point has it be de-initialised or reinitialised when it was popped from the active stack onto the stashed stack or vice versa.

## Game world
	
A game framework layer is pushed - for the initial area - the players house. The layer will be called a "TilemapAndSpriteGameLayer".
When a new area is entered - such as leaving the house - a new layer will be  pushed on top of the stack. First a list of layers will be searched to check if the new area is already on the game stack. If it is, the stack will be popped until the area is reache. The aforementioned "stashed stack" will be useful for pushing the HUD layer onto and popping it off to load new areas . The list of areas that correspond to layers on the game framework stack will be maintained by having the TilemapAndSpriteGameLayer push and pop to a list in its onpush and onpop callbacks.


- TilemapAndSpriteGameLayer
	- sprite and tile atlas
	- list of game objects and components
		- simple game object and component system of some kind
		- Engine level Components:
			- Sprite
			- Sprite animator
			- Physics
			- Player Controller
			- Interactable (lua scripted)
			- Trigger volume (lua scripted)
			- Tile(s) reference - a reference to tiles on one or more tile layers
		- Components query the existence of other components and gain references at initialisation time
	- list of tile layers, in drawn order
	- physics world (box2d)
	- functions to serialize above
	- ID which must be unique per are in the game 
	- This will be a game layer within the engine library
	- Takes a PersistantGameState object when initialised
		- This has a user data pointer, which is the game implementation. This will be game specific stuff not covered by existance of game objects
		- Default implementation includes layers of tile and game object list overlay to the basic one for specific base area maps, area maps referenced by TilemapAndSpriteGameLayer ID
		- Game objects created at runtime are put into this persistant state object when layer serialzes - default engine implementation

## Rendering
	
	- OpenGL ES 2. Future vulkan perhaps
	- Quad tree for sprite culling
	- For tile rendering, method that generates sprite vertexes in vertex shader from gl_VertexID
	- Single atlas texture for everything rendered in game layer, sprites and tiles, array mapping index to UV offset needed so that lookups to the tile texture can translate to UVs for a tile
	- Better system for only loading needed textures. But it is also convenient to atlas everything needed for a specific area
	- Some basic particle system (possibly)
	- Render sprites from top of screen to bottom to give illusion of 3d, objects in front of player occlude him

## Tooling

	- Use third party tilemap and level editors - Tiled maybe
		- Create tool to build packed binary files for the game like tar files including assets as binary and text data
	- Live reloading UI viewer
		- Watches xml file and dependencies for changes, reloads UI, displays errors
	- In game:
		- Terminal (in progress) - call lua exposed functions

## Engine improvements
	- string tables / utf8
















