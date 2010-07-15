/*
[17:15:52] <messiaen> spinout
[17:15:57] <messiaen> to get initial position
[17:16:04] <messiaen> read x,y,z coordinates of an object with behavior 0x13002F74
[17:16:09] <messiaen> created with the command 0x2418
[17:16:42] <spinout> i like makefiles
[17:16:48] <messiaen> or 130056A4

[24] [18] [1F] [36] [0D 3E] [00 00] [01 80] [00 00] [00 B4] [00 00] [00 00 00 00] [13] [00 42 84]

[1]: 24= Places an object in level at X Y Z position and rotation, with some parameters.
[2]: Length byte (dec 24)
[3]: Course(s) on which the object will appear, as binary value
[4]: ID number of the object, as defined by a previous 0x22 command
[5,6] [7,8] [9,10]: X Y and Z position in level as 16-bit signed integers
[11,12] [13,14] [15,16]: X Y and Z rotation as 16-bit signed integers
[17,18,19,20]: Parameters to be fed into the behavior code, usage varies depending on which behavior
[21,22,23,24]: RAM segment number and offset for behavior code to be attached to this object
*/

#include "globals.h"

void SM64_Behavior_To_OoT(unsigned int Segment, unsigned int Pos, unsigned int BehaviorCode)
{
	if(LevelArea != CurrentLevelArea) return;

	// BehaviorCode is zero for 0x2418 object, non-zero for 0x3908 macro object
	bool IsMacroObj = false;
	if(BehaviorCode != 0) IsMacroObj = true;

	signed short x,y,z;
	signed short r;

	bool IsMario = false, IsDoor = false;
	int ZActNum = 0x0008;
	int ZObjNum = 0x0000;
	int ZVariable = (IsMacroObj ? 0x0003 : 0x0000);

	if(IsMacroObj) {
		x = Read16(RAMSegment[Segment].Data, Pos+2);
		y = Read16(RAMSegment[Segment].Data, Pos+4);
		z = Read16(RAMSegment[Segment].Data, Pos+6);
		//what's going on here?
		r = ((((RAMSegment[Segment].Data[Pos]<<8) | RAMSegment[Segment].Data[Pos+1]) & 0xF000) >> 12) * 24;	//0xFE00
	} else {
		x = Read16(RAMSegment[Segment].Data, Pos+4);
		y = Read16(RAMSegment[Segment].Data, Pos+6);
		z = Read16(RAMSegment[Segment].Data, Pos+8);
		r = Read16(RAMSegment[Segment].Data, Pos+12);
	}

	x /= SCALE;
	y /= SCALE;
	z /= SCALE;

	r *= 182; // SM64 rotation is -180 : 180; whereas Z64 is 32767 : -32767; 180 * 182 = 32760

	if(!IsMacroObj)
		BehaviorCode = Read32(RAMSegment[Segment].Data, Pos+20);

	switch(BehaviorCode){
	case STARDOOR:
		IsDoor=true;
		ZActNum = 0x000E;
		break;
	case MRI:
		ZActNum = 0x0008;
		break;
	case POLEGRABBING:
		ZActNum = 0x0008;
		break;
	case THISLANDTOPTRAP:
		ZActNum = 0x0008;
		break;
	case TINYISLANDTOP:
		ZActNum = 0x0008;
		break;
	case ACTIVATECAPSWITCH:
		ZActNum = 0x0008;
		break;
	case KINGBOBOMB:
		ZActNum = 0x0008;
		break;
	case OPENEDCANNON:
		ZActNum = 0x0008;
		break;
	case CHUCKYA:
		ZActNum = 0x0008;
		break;
	case ROTATINGPLATFORM:
		ZActNum = 0x0008;
		break;
	case TOWERBEHAV:
		ZActNum = 0x0008;
		break;
	case BULLETBILLCANNONBEHAV:
		ZActNum = 0x0008;
		break;
	case BREAKABLEWALL:
		ZActNum = 0x0008;
		break;
	case BREAKABLEWALL2:
		ZActNum = 0x0008;
		break;
	case KICKABLEBOARD:
		ZActNum = 0x0008;
		break;
	case TOWERDOORBEHAV:
		IsDoor=true;
		ZActNum = 0x000E;
		break;
	case CLOCKLIKEROTATION:
		ZActNum = 0x0008;
		break;
	case KOOPASHELLPOWERUP:
		ZActNum = 0x0008;
		break;
	case EXITPODIUMWARP:
		ZActNum = 0x0008;
		break;
	case FADINGWARP:
		ZActNum = 0x0008;
		break;
	case WARP:
		ZActNum = 0x0008;
		break;
	case WARPPIPE:
		ZActNum = 0x0008;
		break;
	case COINFORMATION:
		ZActNum = 0x01AB;
		break;
	case DOORWARP:
		IsDoor=true;
		ZActNum = 0x000E;
		break;
	case DOOR:
		IsDoor=true;
		ZActNum = 0x000E;
		break;
	case GRINDEL:
		ZActNum = 0x0008;
		break;
	case THWOMP2:
		ZActNum = 0x0008;
		break;
	case THWOMP:
		ZActNum = 0x0008;
		break;
	case TUMBLINGBRIDGE:
		ZActNum = 0x0008;
		break;
	case TUMBLINGBRIDGE_:
		ZActNum = 0x0008;
		break;
	case BURNING:
		ZActNum = 0x0008;
		break;
	case MOVINGSOLIDBEHAV:
		ZActNum = 0x0008;
		break;
	case ELEVATOR:
	case ELEVATOR_:
	case ELEVATOR__:
		ZActNum = 0x0008;
		break;
	case WATERMISTBEHAV:
		ZActNum = 0x0008;
		break;
	case PEACHATTHEEND:
		ZActNum = 0x0008;
		break;
	case UKIKI:
		ZActNum = 0x0008;
		break;
	case LITTLECAGE:
		ZActNum = 0x0008;
		break;
	case SINKINGPLATFORMS:
		ZActNum = 0x0008;
		break;
	case SINKINGPLATFORM:
		ZActNum = 0x0008;
		break;
	case TILTINGPLATFORM:
		ZActNum = 0x0008;
		break;
	case SQUISHABLEPLATFORMS:
		ZActNum = 0x0008;
		break;
	case ROTATION2:
		ZActNum = 0x0008;
		break;
	case FLAMETHROWER:
		ZActNum = 0x0008;
		break;
	case JUMPINGFIREBALL:
		ZActNum = 0x0008;
		break;
	case MULTIPLECOINS:
		ZActNum = 0x01AB;
		break;
	case SPINDRIFT:
		ZActNum = 0x0008;
		break;
	case MOVINGONASQUARISHPATH:
		ZActNum = 0x0008;
		break;
	case FLOORSWITCHANIMATESBEHAV20128:
		ZActNum = 0x0008;
		break;
	case FLOORSWITCHFORGRILLS:
		ZActNum = 0x0008;
		break;
	case FLOORSWITCHSHOWSHIDDENOBJECTS:
		ZActNum = 0x0008;
		break;
	case HIDDENOBJECT:
		ZActNum = 0x0008;
		break;
	case BREAKABLEBOX:
	case BREAKABLEBOX_:
	case JUMPINGBOX:
	case EXCLAMATIONBOX:
		ZActNum = 0x01A0;
		ZObjNum = 0x0170;
		break;
	case PUSHABLE:
		ZActNum = 0x0008;
		break;
	case HEAVEHO:
		ZActNum = 0x0008;
		break;
	case CABINDOOR:
		IsDoor=true;
		ZActNum = 0x000E;
		break;
	case IGLOOTHING:
		ZActNum = 0x0008;
		break;
	case BULLETBILLBEHAV:
		ZActNum = 0x0008;
		break;
	case BOWSER:
		ZActNum = 0x0008;
		break;
	case TILTINGBOWSERLAVAPLATFORM:
		ZActNum = 0x0008;
		break;
	case FALLINGBOWSERPLATFORM:
		ZActNum = 0x0008;
		break;
	case UNKNOWN3IC:
		ZActNum = 0x0008;
		break;
	case ELEVATORS:
		ZActNum = 0x0008;
		break;
	case CASTLEGROUND:
		ZActNum = 0x0008;
		break;
	case POUNDFOREVENT:
		ZActNum = 0x0008;
		break;
	case ROTATINGCLOCKARM:
		ZActNum = 0x0008;
		break;
	case ROTATINGSMALLCLOCKARM:
		ZActNum = 0x0008;
		break;
	case ROTATINGCCW:
		ZActNum = 0x0008;
		break;
	case SINKSWHENSTEPPINGONIT:
		ZActNum = 0x0008;
		break;
	case MOVINGHORIZONTALLY:
		ZActNum = 0x0008;
		break;
	case ROTATINGCWWITHFIREBARS:
		ZActNum = 0x0008;
		break;
	case FLOATINGWOODBRIDGE:
		ZActNum = 0x0008;
		break;
	case ROTATIONAROUNDYAXIS:
		ZActNum = 0x0008;
		break;
	case SINKINGINLAVA:
		ZActNum = 0x0008;
		break;
	case TILTINGSQUARE:
		ZActNum = 0x0008;
		break;
	case TILTINGINVERTEDPYRAMID:
		ZActNum = 0x0008;
		break;
	case TOXBOX:
		ZActNum = 0x0008;
		break;
	case PIRANHAPLANTBEHAV2:
		ZActNum = 0x0008;
		break;
	case SOLID:
		ZActNum = 0x0008;
		break;
	case TUXIESMOTHER:
		ZActNum = 0x0008;
		break;
	case SMALLPENGUIN:
		ZActNum = 0x0008;
		break;
	case FISHGROUP:
	case FISHGROUP_:
	case FISHGROUP__:
	case AQUARIUMFISHGROUP:
		ZActNum = 0x0021;
		break;
	case PLATFORMSWIVEL2:
		ZActNum = 0x0008;
		break;
	case PLATFORMSWIVEL:
		ZActNum = 0x0008;
		break;
	case CHIRPCHIRP:
		ZActNum = 0x0008;
		break;
	case ROCKSOLID:
		ZActNum = 0x0008;
		break;
	case BOWSERSUBDOOR:
		IsDoor=true;
		ZActNum = 0x000E;
		break;
	case BOWSERSSUB:
		ZActNum = 0x0008;
		break;
	case EXPLODINGBOX:
		ZActNum = 0x0008;
		break;
	case SHIPPART3:
		ZActNum = 0x0008;
		break;
	case INSUNKENSHIP3:
		ZActNum = 0x0008;
		break;
	case SUNKENSHIPPART:
		ZActNum = 0x0008;
		break;
	case SUNKENSHIPPART2:
		ZActNum = 0x0008;
		break;
	case INSUNKENSHIP:
		ZActNum = 0x0008;
		break;
	case INSUNKENSHIP2:
		ZActNum = 0x0008;
		break;
	case BLUECOINSSWITCHING:
	case HIDDENBLUECOIN:
	case ONECOIN:
	case COLLECTABLECOIN:
		ZActNum = 0x0015;
		break;
	case OPENABLEGRILLS:
		ZActNum = 0x0008;
		break;
	case WATERLEVELTRIGGER:
		ZActNum = 0x0008;
		break;
	case UNKNOWNWDW:
		ZActNum = 0x0008;
		break;
	case TWEESTERBEHAV:
		ZActNum = 0x0008;
		break;
	case BOOINCASTLE:
		ZActNum = 0x0008;
		break;
	case BOOS:
		ZActNum = 0x0008;
		break;
	case PATROLLINGBOOBUDDY:
		ZActNum = 0x0008;
		break;
	case BOOBUDDY:
		ZActNum = 0x0008;
		break;
	case BOOBUDDY3:
		ZActNum = 0x0008;
		break;
	case BOOBUDDY2:
		ZActNum = 0x0008;
		break;
	case STAIRCASESTEP:
		ZActNum = 0x0008;
		break;
	case TUMBLINGBOOKSHELF:
		ZActNum = 0x0008;
		break;
	case ROTATINGMERRYGOROUND:
		ZActNum = 0x0008;
		break;
	case STATICNOTSOLID:
		ZActNum = 0x0008;
		break;
	case UNKNOWNIC:
		ZActNum = 0x0008;
		break;
	case TREE:
		ZObjNum = 0x007C;
		ZActNum = 0x0077;
		break;
	case SCUTLEBUG:
		ZActNum = 0x0008;
		break;
	case UNKNOWNHMC:
		ZActNum = 0x0008;
		break;
	case WHOMPKINGBOSSBEHAV:
		ZActNum = 0x0008;
		break;
	case SMALLWHOMP:
		ZActNum = 0x0008;
		break;
	case WINDBLOWINGSNOWMAN:
		ZActNum = 0x0008;
		break;
	case WALKINGPENGUIN:
		ZActNum = 0x0008;
		break;
	case TOADMESSAGE:
		ZActNum = 0x0008;
		break;
	case MOVINGBLUECOIN:
		ZActNum = 0x0015;
		ZVariable = 0x0001;
		break;
	case BUBBLES:
		ZActNum = 0x0007;
		ZVariable = 0x0004;
		break;
	case PINKBOBOMBOPENINGCANNONBEHAV:
	case PINKBOBOMBWITHMESSAGE:
	case BOBOMB:
	case BOWSERBOMB:
		ZActNum = 0x004C;
		ZObjNum = 0x0031;
		break;
	case CANNONTRAPDOOR:
		ZActNum = 0x0008;
		break;
	case MESSAGEPANEL:
		ZActNum = 0x0141;
		ZObjNum = 0x012F;
		ZVariable = 0x0000;
		break;
	case SIGNONWALL:
		ZActNum = 0x0008;
		break;
	case AMP2:
		ZActNum = 0x0008;
		break;
	case AMP:
		ZActNum = 0x0008;
		break;
	case BUTTERFLYBEHAV:
		ZActNum = 0x0094;
		break;
	case OWL:
		ZActNum = 0x014D;
		ZObjNum = 0x0131;
		break;
	case SMALLBULLY:
		ZActNum = 0x0008;
		break;
	case BIGBULLYBOSS:
		ZActNum = 0x0008;
		break;
	case BULLY:
		ZActNum = 0x0008;
		break;
	case CHILLBULLYBOSS:
		ZActNum = 0x0008;
		break;
	case MANTARINGSGENERATOR:
		ZActNum = 0x0008;
		break;
	case BASCULEBRIDGE:
		ZActNum = 0x0008;
		break;
	case MOVESBACKFORTH1:
		ZActNum = 0x0008;
		break;
	case MOVESBACKFORTH2:
		ZActNum = 0x0008;
		break;
	case SLIDINGPLATFORM2:
		ZActNum = 0x0008;
		break;
	case MONEYBAGBEHAV:
		ZActNum = 0x0008;
		break;
	case ROLLINGBALL:
		ZActNum = 0x0008;
		break;
	case METALBALLGENERATOR2:
		ZActNum = 0x0008;
		break;
	case METALBALLGENERATOR:
		ZActNum = 0x0008;
		break;
	case MINIMETALBALLSGENERATOR:
		ZActNum = 0x0008;
		break;
	case FLAPPINGWING:
		ZActNum = 0x0008;
		break;
	case SPINDEL:
		ZActNum = 0x0008;
		break;
	case MOVINGUPANDDOWN:
		ZActNum = 0x0008;
		break;
	case PYRAMIDELEVATOR:
		ZActNum = 0x0008;
		break;
	case PYRAMIDTOPBEHAV:
		ZActNum = 0x0008;
		break;
	case CASTLEFLAGWAVING:
		ZActNum = 0x0008;
		break;
	case CAMERAANCHOR:
		ZActNum = 0x0008;
		break;
	case LISREAL2041:
		ZActNum = 0x0008;
		break;
	case HIDDENAT120STARS:
		ZActNum = 0x0008;
		break;
	case SNOWMANSBOTTOM:
		ZActNum = 0x0008;
		break;
	case SNOWMANSHEADBEHAV:
		ZActNum = 0x0008;
		break;
	case DROPSGIANTBOULDERS:
		ZActNum = 0x0008;
		break;
	case COLLECTSTAR:
		ZActNum = 0x0015;
		ZVariable = 0x0006;
		break;
	case HIDDENREDCOINSTAR:
		ZActNum = 0x0015;
		break;
	case REDCOIN:
		ZActNum = 0x0015;
		break;
	case WINDY:
		ZActNum = 0x0008;
		break;
	case HIDDENSTAR:
		ZActNum = 0x0008;
		break;
	case PITOUNE2:
		ZActNum = 0x0008;
		break;
	case PITOUNE:
		ZActNum = 0x0008;
		break;
	case SLIDING1UP:
	case ONEUP:
	case HIDDEN1UP:
	case HIDDEN1UPINTREE:
		ZActNum = 0x0015;
		ZVariable = 0x0003;
		break;
	case HIDDEN1UPTRIGGER:
		ZActNum = 0x0008;
		break;
	case CONTROLABLEPLATFORM:
		ZActNum = 0x0008;
		break;
	case SNOWWAVESGENERATOR:
		ZActNum = 0x0008;
		break;
	case FLOATINGONWATER:
		ZActNum = 0x0008;
		break;
	case FLOATING:
		ZActNum = 0x0008;
		break;
	case ARROWLIFT:
		ZActNum = 0x0008;
		break;
	case FALLINGPILLAR:
		ZActNum = 0x0008;
		break;
	case PILLARBASEBEHAV:
		ZActNum = 0x0008;
		break;
	case OSCILLATINGPENDULUM:
		ZActNum = 0x0008;
		break;
	case TREASURECHESTSSHIP:
		ZActNum = 0x0008;
		break;
	case TREASURECHESTSJOLLY:
		ZActNum = 0x0008;
		break;
	case TREASURECHESTS:
		ZActNum = 0x0008;
		break;
	case MIPSRABBIT:
		ZActNum = 0x0008;
		break;
	case YOSHI:
		ZActNum = 0x0008;
		break;
	case KOOPA:
		ZActNum = 0x0008;
		break;
	case WAVINGKOOPAFLAG:
		ZActNum = 0x0008;
		break;
	case POKEY:
		ZActNum = 0x0008;
		break;
	case BAT:
		ZActNum = 0x01C0;
		ZObjNum = 0x0008;
		break;
	case FLYGUY:
		ZActNum = 0x0008;
		break;
	case GOOMBAWALK:
		ZActNum = 0x001B;
		ZObjNum = 0x0016;
		break;
	case THREEGOOMBAS:
		ZActNum = 0x0008;
		break;
	case CHAINCHOMP:
		ZActNum = 0x0002;
		ZObjNum = 0x0032;
		ZVariable = 0x0003;
		break;
	case POUNDABLE:
		ZActNum = 0x0008;
		break;
	case EXPLODINGWHENCHAINCHOMPISFREED:
		ZActNum = 0x0008;
		break;
	case WIGGLER:
		ZActNum = 0x0008;
		break;
	case EVILLAKITU:
		ZActNum = 0x0008;
		break;
	case LAKITU:
		ZActNum = 0x0008;
		break;
	case FWOOSHBLOWINGWIND:
		ZActNum = 0x0008;
		break;
	case MONTYMOLE:
		ZActNum = 0x0008;
		break;
	case MONTYMOLEINHOLE:
		ZActNum = 0x0008;
		break;
	case PLATFORMONTRACKS:
		ZActNum = 0x0008;
		break;
	case SOLIDMOVING:
		ZActNum = 0x0008;
		break;
	case FOURROTATINGPLATFORMS:
		ZActNum = 0x0008;
		break;
	case WATERBUBBLEDROP:
		ZActNum = 0x0008;
		break;
	case ROTATINGCLOCKPLATFORM:
		ZActNum = 0x0008;
		break;
	case CLOCKPENDULUM:
		ZActNum = 0x0008;
		break;
	case CLOCKMETALPLATFORM:
		ZActNum = 0x0008;
		break;
	case SLIDINGPLATFORM:
		ZActNum = 0x0008;
		break;
	case ROTATINGGEARPLATFORM:
		ZActNum = 0x0008;
		break;
	case PUSHABLECLOCKBOX:
		ZActNum = 0x0008;
		break;
	case CLOCKPLATFORM:
		ZActNum = 0x0008;
		break;
	case CLOCKMAINROTATION:
		ZActNum = 0x0008;
		break;
	case ROTATINGCLOCKPLATFORM_:
		ZActNum = 0x0008;
		break;
	case MRBLIZZARD:
		ZActNum = 0x0008;
		break;
	case SLIDINGPLATFORM_:
		ZActNum = 0x0008;
		break;
	case ROTATION:
		ZActNum = 0x0008;
		break;
	case ANIMATESONFLOORSWITCHPRESS:
		ZActNum = 0x0008;
		break;
	case MOVESWHENSTEPPINGONIT:
		ZActNum = 0x0008;
		break;
	case RECOVERLIFE:
		ZActNum = 0x0008;
		break;
	case CANNON:
		ZActNum = 0x0008;
		break;
	case DORRIE:
		ZActNum = 0x0008;
		break;
	case HAUNTEDCHAIR:
		ZActNum = 0x0008;
		break;
	case MADPIANO:
		ZActNum = 0x0008;
		break;
	case FLYINGBOOKEND:
		ZActNum = 0x0008;
		break;
	case BOOKSHELFTHING:
		ZActNum = 0x0008;
		break;
	case PIRANHAPLANT:
		ZActNum = 0x0008;
		break;
	case FIRESPEWER:
		ZActNum = 0x0008;
		break;
	case SNUFIT:
		ZActNum = 0x0008;
		break;
	case GRINDEL_:
		ZActNum = 0x0008;
		break;
	case EYEROKBOSS:
		ZActNum = 0x0008;
		break;
	case KLEPTOBEHAV:
		ZActNum = 0x0008;
		break;
	case MULTIPLEBIRDSFLYING:
		ZActNum = 0x0008;
		break;
	case RACINGPENGUIN:
		ZActNum = 0x0008;
		break;
	case CLAMSHELLBEHAV:
		ZActNum = 0x0008;
		break;
	case SKEETER:
		ZActNum = 0x0008;
		break;
	case PENDULUMLIKEMOVEMENT:
		ZActNum = 0x0008;
		break;
	case BUTTERFLY:
		ZActNum = 0x0094;
		break;
	case BUBBA:
		ZActNum = 0x0008;
		break;
	case PEACH:
		ZActNum = 0x0008;
		break;
	case MARIOS:
	case WARPMARIOSTART:
	case 0x13002F74:
		IsMario = true;
		break;
	}

	if (IsMacroObj) {
		dmsg("- Macro Object: ");
	}
	if (IsMario) {
		dmsg("- Mario -> Link");
		ZLink(ZLinkBuffer, 0, x, y, z, 0, r, 0, 0xFFF);
	}
	else if (IsDoor) {
		dmsg("- Ignoring door [%08X]", BehaviorCode);
	}
	else if (ZActorCount < 0x3F) {
		dmsg("- Behavior %08X -> Actor %04X", BehaviorCode, ZActNum);
		ZActor(ZActorBuffer, (ZActorCount<<4), ZActNum, x, y, z, 0, r, 0, ZVariable);
		ZActorCount++;
	}
	dmsg(" (% 5i, % 5i, % 5i - % 6i)\n", x, y, z, r);
	/* Objects */
	if (ZObjNum > 3 && ZObjectCount < 15) {
		int i, TmpObjNum;
		for (i=0;i<ZObjectCount<<1;i+=2) {
			TmpObjNum = Read16(ZObjectBuffer, i);
			if( TmpObjNum == ZObjNum )
				return;
		}
		dmsg("- Object %04X requested and supplied\n", ZObjNum);
		Write16(ZObjectBuffer, i+0, ZObjNum);
		ZObjectCount++;
	}
}
