#include "bosses.h"

Boss::Boss(std::string const p_name, std::vector<uint32_t> p_ids = std::vector<uint32_t>(), int64_t p_timer = 0, uint64_t p_health = 0)
{
	ids = p_ids;
	name = p_name;
	timer = p_timer;
	health = p_health;
	pulls = 0;
}

bool Boss::hasId(uint32_t new_id)
{
    for(uint16_t index=0;index<ids.size();index++)
    {
        if(new_id == ids.at(index))
        {
            return true;
        }
    }
    return false;
}

bool Boss::operator==(Boss * other_boss)
{
	return other_boss && ids.size()>0 && other_boss->ids.size()>0 
		&& ids[0] == other_boss->ids[0];
}

Boss boss_generic = Boss("Generic");
Boss boss_vg = Boss("Vale Guardian", { 0x3C4E }, 8 * 60 * 1000, 22021440);
Boss boss_gors = Boss("Gorseval the Multifarious", { 0x3C45 }, 7 * 60 * 1000, 21628200);
Boss boss_sab = Boss("Sabetha the Saboteur", { 0x3C0F }, 9 * 60 * 1000, 34015256);
Boss boss_sloth = Boss("Slothasor", { 0x3EFB }, 7 * 60 * 1000, 18973828);
Boss boss_trio = Boss("Bandit Trio", { 0x3ED8,0x3F09,0x3EFD }, 9 * 60 * 1000);
Boss boss_matti = Boss("Matthias Gabrel", { 0x3EF3 }, 10 * 60 * 1000, 25953840);
Boss boss_kc = Boss("Keep Construct", { 0x3F6B }, 10 * 60 * 1000, 55053600);
Boss boss_xera = Boss("Xera", { 0x3F76,0x3F9E }, 11 * 60 * 1000, 22611300);
Boss boss_cairn = Boss("Cairn the Indomitable", { 0x432A }, 8 * 60 * 1000, 19999998);
Boss boss_mo = Boss("Mursaat Overseer", { 0x4314 }, 6 * 60 * 1000, 22021440);
Boss boss_sam = Boss("Samarog", { 0x4324 }, 11 * 60 * 1000, 29493000);
Boss boss_deimos = Boss("Deimos", { 0x4302 }, 12 * 60 * 1000, 50049000);
Boss boss_sh = Boss("Soulless Horror", { 0x4D37 }, 8 * 60 * 1000, 35391600);
Boss boss_soul_eater = Boss("Statues - Soul Eater", { 0x4C50 }, 1720425);
Boss boss_ice_king = Boss("Statues - Ice King", { 0x4CEB }, (3 * 60 + 30) * 1000, 9831000);
Boss boss_cave = Boss("Statues - Cave", { 0x4CC3,0x4D84 }, 2457750);//North: Eye of Judgement, South: Eye of Fate
Boss boss_dhuum = Boss("Dhuum", { 0x4BFA }, 10 * 60 * 1000, 32000000);
Boss boss_ca = Boss("Conjured Amalgamate", { 43974,37464,10142 }, 8 * 60 * 1000, 52290000);
Boss boss_largos = Boss("Twin Largos", { 21105, 21089 }, 2 * 4 * 60 * 1000, 17548336);//using Nikare (left side hp)
Boss boss_qadim = Boss("Qadim", { 20934 }, 13 * 60 * 1000, 19268760);
Boss boss_adina = Boss("Adina", { 22006 }, 8 * 60 * 1000, 22611300);
Boss boss_sabir = Boss("Sabir", { 21964 }, 29493000);
Boss boss_qadim2 = Boss("Qadim the Peerless", { 22000 }, 12 * 60 * 1000, 47188800);

Boss boss_fotm_generic = Boss("FotM Generic");
Boss boss_mama = Boss("MAMA", { 0x427D,0x4268,0x424E }, 5200519);//ids: cm,normal,knight at the start of the trash on CM
Boss boss_siax = Boss("Siax", { 0x4284 }, 6138797);//TODO get normal mode id
Boss boss_ensolyss = Boss("Ensolyss of the Endless Torment", { 0x4234 }, 14059890);//TODO get normal mode id
Boss boss_skorvald = Boss("Skorvald the Shattered", { 0x44E0 }, 5551340);//TODO get normal mode id
Boss boss_artsariiv = Boss("Artsariiv", { 0x461D }, 5962266);//TODO get normal mode id
Boss boss_arkk = Boss("Arkk", { 0x455F }, 9942250);//TODO get normal mode id

Boss boss_strike_generic = Boss("Strike Generic");
Boss boss_boneskinner = Boss("Boneskinner", { 22521 });//TODO get timer & health
Boss boss_fraenir = Boss("Fraenir", { 22492 });//TODO get timer & health
Boss boss_kodan = Boss("Kodan Brothers", { 22343, 22481, 22315 });//TODO get timer & health
Boss boss_whisper = Boss("Whisper of Jormag", { 22711 });//TODO get timer & health
Boss boss_icebrood_construct = Boss("Icebrood Construct", { 22154, 22436 });//TODO get timer & health

std::list<Boss*> bosses =
{
	&boss_generic,
	&boss_vg,
	&boss_gors,
	&boss_sab,
	&boss_sloth,
	&boss_trio,
	&boss_matti,
	&boss_kc,
	&boss_xera,
	&boss_cairn,
	&boss_mo,
	&boss_sam,
	&boss_deimos,
	&boss_sh,
	&boss_soul_eater,
	&boss_ice_king,
	&boss_cave,
	&boss_dhuum,
	&boss_ca,
	&boss_largos,
	&boss_qadim,
	&boss_fotm_generic,
	&boss_mama,
	&boss_siax,
	&boss_ensolyss,
	&boss_skorvald,
	&boss_artsariiv,
	&boss_arkk,
	&boss_boneskinner,
	&boss_fraenir,
	&boss_kodan,
	&boss_whisper,
	&boss_icebrood_construct,
};
