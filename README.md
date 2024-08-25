# ICP Projekt Kohlík, Čaloun
Autoři: Martin Kohlík, Petr Čaloun
-------------------------------------------------------------------------------------------------
## Grafická část
V grafické části hry je prostředí ohraničené zdí, za kterou je textura okolí. Po herní ploše se hráč může pohybovat, ale musí utíkat před zajícem, který jej pronásleduje. Každý pohyb má vlastní audio krok, stejně tak kolize se zajícem je doprovázená vlastním zvukovým efektem

Ovládání
- Klávesy W, S, A, D pro pohyb
- Myš pro rozhlížení
- Kolečko myši pro změnu FOV
- F pro přepínání mezi režimem okna a fullscreen
- ESC pro ukončení programu

## Rastrová část
Při spuštění programu se pokusí otevřít kameru, na které křížkem označuje červenou barvu na objektu. V případě, že se kameru nepodaří otevřít, otevře místo toho video

Rastrová a grafická část jsou řešeny jako oddělené projekty

## Sestavení
Pro sestavení projektů je potřeba mít vytvořenou systémovou proměnnou OPENCV_DIR, která směřuje do složky /build opencv. A do PATH přidat %OPENCV_DIR%\x64\vc16\bit. Zbylé závislosti jsou řešeny v rámci projektu a jsou součástí repozitáže (glew, glfw…). Pro plnou funkci zpracování obrazu je nutné mít k počítači připojenou kameru, ale v případě nedostupnosti kamery se automaticky spouští video.