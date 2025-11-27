# Combat Ops Enhanced 2 (COE2)

[![Version](https://img.shields.io/github/release/Kexanone/COE2_AR.svg?label=Version&colorB=007EC6&style=flat-square)](https://github.com/Kexanone/COE2_AR/releases/latest)
[![Issues](https://img.shields.io/github/issues-raw/Kexanone/COE2_AR.svg?label=Issues&style=flat-square)](https://github.com/Kexanone/COE2_AR/issues)
[![License](https://img.shields.io/badge/License-APL--SA-orange.svg?style=flat-square)](https://github.com/Kexanone/COE2_AR/blob/main/LICENSE.md)

Co-operative infantry gameplay with dynamic objectives for [Arma Reforger](https://reforger.armaplatform.com/). Next generation of [Combat Ops Enhanced](https://github.com/Kexanone/CombatOpsEnhanced_AR) with inspiration from [mbrdmn's Dynamic Recon Ops from Arma 3](https://steamcommunity.com/workshop/filedetails/?id=722652837).

## Features
- **Replayability** - Dynamic generation of AOs
- **Increased cooperativity** - Game mode is structured in (_briefing_)\*, execution, exfiltration, (_debriefing_)\* and intermission
- **Ingame Configurability** - Scenario and AOs can be configured ingame by a votable commander role
- **Compatibility** - Designed to be compatible with modded factions out of the box

\* Not yet implemented

## Workshop Link

https://reforger.armaplatform.com/workshop/60926835F4A7B0CA

## Mods JSON

```json
{
    "modId": "60926835F4A7B0CA",
    "name": "COE2"
}
```

## Scenario IDs

- COE2 - Arland: `"{F239FD0036BD2C1E}Missions/COE2_Arland.conf"`
- COE2 - Everon: `"{EE676FAB9DFA4CF7}Missions/COE2_Eden.conf"`
- COE2 - Kolguyev: `"{4D4780DB775BFF64}Missions/COE2_Cain.conf"`

## Mission Header

```json
"missionHeader":
{
    "m_sCOE_DefaultPlayerFactionKey": "",
    "m_sCOE_DefaultEnemyFactionKey": "",
    "m_sCOE_DefaultCivilianFactionKey": "",
    "m_eCOE_DefaultEnemyAiSkill": 50,
    "m_fCOE_DefaultAORadius": -1,
    "m_iCOE_DefaultMinEnemyAICount": -1,
    "m_fCOE_DefaultEnemyAICountMultiplier": -1,
    "m_iCOE_EnemyArmedVehicleCount": 0,
    "m_bCOE_EnemySupportEnabled": false,
    "m_fCOE_MinEnemyReinforcementTime": 5,
    "m_fCOE_MaxEnemyReinforcementTime": 15,
    "m_bCOE_CiviliansEnabled": true,
    "m_bCOE_CommanderBecomesGM": false
}
```

## Optional Addons

- COE2 Anizay: https://reforger.armaplatform.com/workshop/65734D75BC584950
- COE2 Denali: https://reforger.armaplatform.com/workshop/657407A89697A1BF
- COE2 Fallujah: https://reforger.armaplatform.com/workshop/21209DA9CF32FBE9
- COE2 Khanh Trung: https://reforger.armaplatform.com/workshop/61C57D80C78AE1C1
- COE2 Kunar Province: https://reforger.armaplatform.com/workshop/61BB79ADF3596AEA
- COE2 Mussalo: https://reforger.armaplatform.com/workshop/61C7786BB6B29693
- COE2 Nizla Island: https://reforger.armaplatform.com/workshop/6649632799B81411
- COE2 Novka: https://reforger.armaplatform.com/workshop/65E57B4395A36FE9
- COE2 Ruha: https://reforger.armaplatform.com/workshop/65EB440190E0B2DF
- COE2 Worthy Islands: https://reforger.armaplatform.com/workshop/61C769418AA32E81
- COE2 Zimnitrita: https://reforger.armaplatform.com/workshop/6311625CDE04C97B

## Optional Scenario IDs

- COE2 Anizay: `{C36DE963B8856E2B}Missions/COE2_Anizay.conf`
- COE2 Denali: `{042F5F64C6D96785}Missions/COE2_Denali.conf`
- COE2 Fallujah: `{B70881ACF0DB5033}Missions/COE2_Fallujah.conf`
- COE2 Khanh Trung: `{9BA76CEE010CCF5C}Missions/COE2_KhanhTrung.conf`
- COE2 Kunar Provincey: `{8DAFAB41500D7985}Missions/COE2_Kunar.conf`
- COE2 Mussalo: `{6EADDAED4B365710}Missions/COE2_Mussalo.conf`
- COE2 Nizla Island: `{A168E25F63A7DD82}Missions/COE2_NizlaIsland.conf`
- COE2 Novka: `{9728C488D74C7CF5}Missions/COE2_Novka.conf`
- COE2 Ruha: `{645FDAD743C488D4}Missions/COE2_Ruha.conf`
- COE2 Worthy Islands: `{CC1F3802A3EDDBD1}Missions/COE2_WorthyIslands.conf`
- COE2 Zimnitrita: `{8E801F0D8414B499}Missions/COE2_Zimnitrita.conf`

## Compatibility Addons

- COE2 - RHS Patch: https://reforger.armaplatform.com/workshop/61BCC264BD3DB22E

## Submitting Issues

Please use our [Issue Tracker](https://github.com/Kexanone/COE2_AR/issues/new/choose) to report a bug, propose a feature, or suggest changes to the existing ones.

## Acknowledgements
- Includes OST from [Arma Licensed Data Pack A2 and A2OADLC](https://www.bohemia.net/community/licenses/licensed-data-packages).
- Includes parachute model from [Arma 2 Sample Models](https://www.bohemia.net/community/licenses/licensed-data-packages).

## License
This mission is licensed under the Arma Public License Share Alike ([APL-SA](https://github.com/Kexanone/COE2_AR/blob/main/LICENSE.md)).
