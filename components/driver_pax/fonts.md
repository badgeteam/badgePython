
# Existing fonts

## Legacy framebuffer fonts
| Source file				| Framebuffer name			| PAX name				| BPP | Point sizes
| :----------				| :---------------			| :-------				| :-- | :----------
| `org_01.ttf`				| `org01_8`, `org18`		| ?						| 1   | 8
| `fairlight.ttf`			| `fairlight*` 				| Fairlight				| 1   | 8, 12
| `dejavusans.ttf`			| `dejavusans20`			| Dejavu Sans			| 1   | 20
| `permanentmarker.ttf`		| `permanentmarker*`		| Permanent Marker		| 1   | 22, 36
| `roboto-Black.ttf`		| `roboto_black22`			| Roboto Black			| 1   | 22
| `roboto-BlackItalic.ttf`	| `roboto_blackitalic24`	| Roboto Black Italic	| 1   | 24
| `roboto.ttf`				| `roboto_regular*`			| Roboto				| 1   | 12, 18, 22
| `weather.ttf`				| `weather42`				| ?						| 1   | 42
| ?							| `pixelade13`				| ?						| 1   | 13
| N/A						| `7x5`						| ?						| 1   | 7
| `ocra.ttf`				| `ocra*`					| ?						| 1   | 16, 22
| `Exo2-Regular.ttf`		| `exo2_regular*`			| Exo2					| 1   | 12, 18, 22
| `Exo2-Thin.ttf`			| `exo2_thin*`				| Exo2 Thin				| 1   | 12, 18, 22
| `Exo2-Bold.ttf`			| `exo2_bold*`				| Exo2 Bold				| 1   | 12, 18, 22
| `PressStart2P.ttf`		| `press_start_2p*`			| Press Start 2P		| 1   | 6, 8, 9, 12, 18, 22

## PAX builtin fonts
| Source file				| Framebuffer name			| PAX name				| BPP | Point sizes
| :----------				| :---------------			| :-------				| :-- | :----------
| sky_var.json				| `sky9`					| Sky					| 1   | 9
| sky_mono.json				| `sky_mono9` 				| Sky Mono				| 1   | 9
| `PermanentMarker.woff`	| `permanentmarker22`		| Permanent Marker		| 2   | 22
| ?							| `saira_condensed45`		| Saira Condensed		| 2   | 45
| ?							| `saira_regular18`			| Saira Regular			| 2   | 18

# Fonts to use

## Fonts to keep
| Name					| Point sizes	| Storage location
| :---					| :----------	| :---------------
| Sky					| 9				| Built-in
| Sky Mono				| 9				| Built-in
| Saira Regular			| 18			| Built-in
| Saira Condensed		| 45			| FS
| Permanent Marker		| 22			| FS

## Fonts to re-render
| Name					| BPP	| Point sizes	| Storage location
| :---					| :--	| :----------	| :---------------
| Permanent Marker		| 2		| 36			| FS
| Fairlight				| 1		| 8, 12			| FS
| Dejavu Sans			| 2		| 20			| FS
| Roboto Black			| 2		| 22			| FS
| Roboto Black Italic	| 2		| 24			| FS
| Roboto				| 2		| 12, 18, 22	| FS
| Exo2					| 2		| 12, 18, 22	| FS
| Exo2 Thin				| 2		| 12, 18, 22	| FS
| Exo2 Bold				| 2		| 12, 18, 22	| FS
| Press Start 2P		| 1		| 8				| Built-in

<!-- [[32, 126], [128, 255], [8364, 8364], [8482, 8482]] -->
