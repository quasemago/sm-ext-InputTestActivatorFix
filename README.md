#### File: se2007/game/server/game_ui.cpp
##### Line 292:

```C
void CGameUI::Think( void )
[...]
	pPlayer->AddFlag( FL_ONTRAIN );
[...]
```
Replaced **pPlayer->AddFlag( FL_ONTRAIN );** with **NOP** to fix prediction issues
while having game_ui active.
