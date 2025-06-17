###############################################################################
#
#                 Rainlendar installer script for NSIS
#                           version 0.10
#
###############################################################################

#
# Include Modern UI
#
!include "MUI.nsh"
  
#
# Variables
#
Name "Rainlendar 0.20"
OutFile "rainlendar-0.20.exe"
InstallDir "$PROGRAMFILES\Rainlendar"
InstallDirRegKey HKLM SOFTWARE\Rainlendar "Install_Dir"
XPStyle on

#
# Modern UI Configuration
#
!define MUI_HEADERIMAGE
!define MUI_ABORTWARNING

#
# Pages
#
!define MUI_FINISHPAGE_RUN "$INSTDIR\Rainlendar.exe"

!insertmacro MUI_PAGE_COMPONENTS
!insertmacro MUI_PAGE_DIRECTORY
!insertmacro MUI_PAGE_INSTFILES
!insertmacro MUI_PAGE_FINISH

!insertmacro MUI_UNPAGE_CONFIRM
!insertmacro MUI_UNPAGE_INSTFILES

#
# Languages
#
!insertmacro MUI_LANGUAGE "Albanian"
!insertmacro MUI_LANGUAGE "Bulgarian"
!insertmacro MUI_LANGUAGE "Catalan"
!insertmacro MUI_LANGUAGE "Korean"
!insertmacro MUI_LANGUAGE "Croatian"
!insertmacro MUI_LANGUAGE "Czech"
!insertmacro MUI_LANGUAGE "Danish"
!insertmacro MUI_LANGUAGE "Estonian"
!insertmacro MUI_LANGUAGE "English"
!insertmacro MUI_LANGUAGE "Finnish"
!insertmacro MUI_LANGUAGE "French"
!insertmacro MUI_LANGUAGE "German"
!insertmacro MUI_LANGUAGE "Greek"
!insertmacro MUI_LANGUAGE "Hebrew"
!insertmacro MUI_LANGUAGE "Hungarian"
!insertmacro MUI_LANGUAGE "Italian"
# Japanese is missing
!insertmacro MUI_LANGUAGE "Latvian"
# Lithuanian is missing
!insertmacro MUI_LANGUAGE "Dutch"
!insertmacro MUI_LANGUAGE "Norwegian"
!insertmacro MUI_LANGUAGE "Polish"
# Portuguese is missing
!insertmacro MUI_LANGUAGE "PortugueseBR"
!insertmacro MUI_LANGUAGE "Russian"
!insertmacro MUI_LANGUAGE "Serbian"
!insertmacro MUI_LANGUAGE "SimpChinese"
!insertmacro MUI_LANGUAGE "Slovak"
!insertmacro MUI_LANGUAGE "Spanish"
!insertmacro MUI_LANGUAGE "Swedish"
!insertmacro MUI_LANGUAGE "TradChinese"


!insertmacro MUI_RESERVEFILE_LANGDLL

LangString strRemove ${LANG_SERBIAN}         "Da li zelite da uklonite podesavanja i dogadjaje?"
LangString strNotRemoved ${LANG_SERBIAN}     "Napomena: $INSTDIR ne moze biti uklonjen!"
LangString strStartMenuSec ${LANG_SERBIAN}   "Precice u Start meniju"
LangString strDesktopSec ${LANG_SERBIAN}     "Precica na radnoj povrsini"
LangString strStartupSec ${LANG_SERBIAN}     "Precica u Startup-u"
LangString strRainlendarDesc ${LANG_SERBIAN} "Ovo instalira glavni programi i zadata odela i jezike."
LangString strStartMenuDesc ${LANG_SERBIAN}  "Ovo unosi precice u tvoj Start Menu."
LangString strDesktopDesc ${LANG_SERBIAN}    "Ovo unosi precice na tvoju radnu povrsinu."
LangString strStartupDesc ${LANG_SERBIAN}    "Ovo unosi precice u tvoju startup fasciklu tako da se Rainlendar pokrece sa Windows-om."
LangString strClose ${LANG_SERBIAN}          "Molimo vas, zatvorite Rainlendar pre nego sto nastavimo."

LangString strRemove ${LANG_HUNGARIAN}         "El akarja távolítani a beállításokat és az eseményeket?"                                    
LangString strNotRemoved ${LANG_HUNGARIAN}     "Megjegyzés: $INSTDIR nem lehet eltávolítani!"                                               
LangString strStartMenuSec ${LANG_HUNGARIAN}   "Start menü parancsikon"                                                                     
LangString strDesktopSec ${LANG_HUNGARIAN}     "Asztal parancsikon"                                                                         
LangString strStartupSec ${LANG_HUNGARIAN}     "Indítópult parancsikon"                                                                     
LangString strRainlendarDesc ${LANG_HUNGARIAN} "Ez telepíti a programot, az alap bõröket és a nyelvi fájlokat."                             
LangString strStartMenuDesc ${LANG_HUNGARIAN}  "Ez hozzáadja a parancsikont a Start menühöz."                                               
LangString strDesktopDesc ${LANG_HUNGARIAN}    "Ez hozzáadja a parancsikont az Asztalhoz."                                                  
LangString strStartupDesc ${LANG_HUNGARIAN}    "Ez hozzáadja a parancsikont az Indítópulthoz, így a Rainlendar a Windows-al indul."         
LangString strClose ${LANG_HUNGARIAN}          "Kérem zárja be a Rainlendar-t, mielõtt folytatná."                                          

LangString strRemove ${LANG_GREEK} "Èİëåéò íá áöáéñİóåéò ôéò ñõèìßóåéò êáé ôá ãåãïíüôá;"
LangString strNotRemoved ${LANG_GREEK} "Óçìåßùóç: $INSTDIR äåí ìğïñåß íá áöáéñåèåß!"
LangString strStartMenuSec ${LANG_GREEK} "Óõíôïìåıóåéò ìåíïı åêêßíçóçò"
LangString strDesktopSec ${LANG_GREEK} "Óõíôïìåıóåéò åğéöÜíåéáò åñãáóßáò"
LangString strStartupSec ${LANG_GREEK} "Óõíôüìåõóç åêêßíçóçò"
LangString strRainlendarDesc ${LANG_GREEK} "ÅãêáèéóôÜ ôï êıñéï ğñüãñáììá, ôá ğñïêáèïñéóìİíá êåëıöç êáé ãëşóóåò."
LangString strStartMenuDesc ${LANG_GREEK} "ÅãêáèéóôÜ ôéò óõíôïìåıóåéò óôï ìåíïı åêêßíçóçò."
LangString strDesktopDesc ${LANG_GREEK} "ÅãêáèéóôÜ óõíôüìåõóç óôçí åğéöÜíåéá åñãáóßáò."
LangString strStartupDesc ${LANG_GREEK} "ÅãêáèéóôÜ óõíôüìåõóç óôïí öÜêåëï åêêßíçóçò İôóé şóôå ôï Rainlendar íá îåêéíÜ ìå ôá Windows."
LangString strClose ${LANG_GREEK} "Ğáñáêáëş êëåßóå ôï Rainlendar ğñéí óõíå÷ßóåéò."

LangString strRemove ${LANG_KOREAN} "¼³Á¤°ú ÀÌº¥Æ® ³»¿ë±îÁö »èÁ¦ÇÏ½Ã°Ú½À´Ï±î?"
LangString strNotRemoved ${LANG_KOREAN} "¾Ë¸²: $INSTDIR Àº »èÁ¦µÇÁö ¾Ê¾Ò½À´Ï´Ù!"
LangString strStartMenuSec ${LANG_KOREAN} "½ÃÀÛ¸Ş´ºÀÇ ¹Ù·Î°¡±â"
LangString strDesktopSec ${LANG_KOREAN} "¹ÙÅÁÈ­¸éÀÇ ¹Ù·Î°¡±â"
LangString strStartupSec ${LANG_KOREAN} "½ÃÀÛÇÁ·Î±×·¥ÀÇ ¹Ù·Î°¡±â(À©µµ¿ì ½ÃÀÛ½Ã ÀÚµ¿½ÇÇà)"
LangString strRainlendarDesc ${LANG_KOREAN} "¼³Ä¡ÇÁ·Î±×·¥Àº ±âº» ¾ğ¾î¿Í ½ºÅ²À» »ç¿ëÇÕ´Ï´Ù."
LangString strStartMenuDesc ${LANG_KOREAN} "½ÃÀÛ¸Ş´º¿¡ ¹Ù·Î°¡±â¸¦ Ãß°¡ÇÕ´Ï´Ù."
LangString strDesktopDesc ${LANG_KOREAN} "¹ÙÅÁÈ­¸é¿¡ ¹Ù·Î°¡±â¸¦ Ãß°¡ÇÕ´Ï´Ù."
LangString strStartupDesc ${LANG_KOREAN} "½ÃÀÛÇÁ·Î±×·¥¿¡ ¹Ù·Î°¡±â¸¦ Ãß°¡ÇØ¼­ À©µµ¿ì°¡ ½ÃÀÛµÉ ¶§¸¶´Ù ½ÇÇàµÇµµ·Ï ÇÕ´Ï´Ù."
LangString strClose ${LANG_KOREAN} "ÀÌ¹Ì Rainlendar°¡ ½ÇÇàÁßÀÌ¶ó¸é ¼³Ä¡¸¦ À§ÇØ Á¾·áÇØ ÁÖ½Ê½Ã¿À."

LangString strRemove ${LANG_ALBANIAN} "Ju dëshironi të suprimoni konfigurimin e po ashtu ngjarjet ?"
LangString strNotRemoved ${LANG_ALBANIAN} "Vërejtje : $INSTDIR nuk ka mundur të fshihet!"
LangString strStartMenuSec ${LANG_ALBANIAN} "Shkurtore në menynë Start"
LangString strDesktopSec ${LANG_ALBANIAN} "Shkurtore në Byro"
LangString strStartupSec ${LANG_ALBANIAN} "Shkurtore në startim"
LangString strRainlendarDesc ${LANG_ALBANIAN} "Kjo instalon programin kresor gjithashtu pamjen dhe gjuhën e parazgjedhur."
LangString strStartMenuDesc ${LANG_ALBANIAN} "Kjo shton shkurtore në menynë Start."
LangString strDesktopDesc ${LANG_ALBANIAN} "Kjo shton shkurtore në Byro."
LangString strStartupDesc ${LANG_ALBANIAN} "Kjo shton shkurtore në dosjen tuaj të startimit, në menyrë që Rainlendar të startoj me Windows."
LangString strClose ${LANG_ALBANIAN} "Ju lutem mbylleni Rainlendar para se te vazhdoni."

LangString strRemove ${LANG_CATALAN} "Vols esborrar les configuracions i les notes?"
LangString strNotRemoved ${LANG_CATALAN} "Nota: &INSTDIR no pot ser eliminat!"
LangString strStartMenuSec ${LANG_CATALAN} "Drecera al menú d'inici"
LangString strDesktopSec ${LANG_CATALAN} "Drecera a l'escriptori"
LangString strStartupSec ${LANG_CATALAN} "Drecera al iniciar"
LangString strRainlendarDesc ${LANG_CATALAN} "Això instal·la el programa principal i les aparences & llengües per defecte"
LangString strStartMenuDesc ${LANG_CATALAN} "Això afegeix les dreceres al menú d'inici"
LangString strDesktopDesc ${LANG_CATALAN} "Això afegeix la drecera a l'escriptori"
LangString strStartupDesc ${LANG_CATALAN} "Això afegeix la drecera a la teva carpeta d'inici i per tant Rainlendar s'iniciarà amb Windows."
LangString strClose ${LANG_CATALAN} "Si us plau, tanca el Rainlendar abans de continuar."

LangString strRemove ${LANG_CROATIAN} "elite li maknuti postavke i dogağaje?"
LangString strNotRemoved ${LANG_CROATIAN} "Napomena: $INSTDIR se nije mogao maknuti!"
LangString strStartMenuSec ${LANG_CROATIAN} "Kratice u Start izborniku"
LangString strDesktopSec ${LANG_CROATIAN} "Desktop kratice"
LangString strStartupSec ${LANG_CROATIAN} "Startup kratice"
LangString strRainlendarDesc ${LANG_CROATIAN} "Ovo instalira glavni program i uobièajene izglede i jezike."
LangString strStartMenuDesc ${LANG_CROATIAN} "Ovo dodaje kratice u vaš Start izbornik."
LangString strDesktopDesc ${LANG_CROATIAN} "Ovo dodaje kratice na vaš Desktop."
LangString strStartupDesc ${LANG_CROATIAN} "Ovo dodaje kratice u vaš Startup direktorij da se Rainlendar moe pokrenuti s Windowsima."
LangString strClose ${LANG_CROATIAN} "Molimo, zatvorite Rainlendar prije nego što nastavite."

LangString strRemove ${LANG_ESTONIAN} "Kas tahad kustutada ka sätted ja sündmused?"
LangString strNotRemoved ${LANG_ESTONIAN} "NB! $INSTDIR kataloogi ei saanud kustutada!"
LangString strStartMenuSec ${LANG_ESTONIAN} "Start menüü otsetee"
LangString strDesktopSec ${LANG_ESTONIAN} "Desktop otsetee"
LangString strStartupSec ${LANG_ESTONIAN} "Käivita otsetee"
LangString strRainlendarDesc ${LANG_ESTONIAN} "See installeerib põhiprogrammi, nahad ja keeled"
LangString strStartMenuDesc ${LANG_ESTONIAN} "See lisab otseteed Start menüüsse"
LangString strDesktopDesc ${LANG_ESTONIAN} "See lisab otseteed Start Desktopile"
LangString strStartupDesc ${LANG_ESTONIAN} "See lisab otseteed Startup kataloogi, et Rainlendar käivituks koos Windowsi'ga"
LangString strClose ${LANG_ESTONIAN} "Sulge Rainlendar enne kui jätkad!"

LangString strRemove ${LANG_HEBREW} "?äàí àúä øåöä ìäñéø àú ääâãøåú åäàéøåòéí"
LangString strNotRemoved ${LANG_HEBREW} "!ùéí ìá: ìà ğéúï ìäñéø àú ñôøééú äúåëğä"
LangString strStartMenuSec ${LANG_HEBREW} "÷éöåøé ãøê áúôøéè äúçì"
LangString strDesktopSec ${LANG_HEBREW} "÷éöåø ãøê áùåìçï äòáåãä"
LangString strStartupSec ${LANG_HEBREW} "÷éöåø ãøê áäôòìä"
LangString strRainlendarDesc ${LANG_HEBREW} "éú÷éï àú äúåëğä äøàùéú åëï àú äùôåú åäîòèôåú áøéøú äîçãì"
LangString strStartMenuDesc ${LANG_HEBREW} "éåñéó ÷éöåøé ãøê ìúôøéè äúçì"
LangString strDesktopDesc ${LANG_HEBREW} "éåñéó ÷éöåø ãøê áùåìçï äòáåãä ùìê"
LangString strStartupDesc ${LANG_HEBREW} "éåñéó ÷éöåø ãøê ìñôøééú ääôòìä ëê ùäúåëğä úåôòì éçã òí çìåğåú"
LangString strClose ${LANG_HEBREW} "àğà ñâåø àú äúåëğä ìôğé ùúîùéê"

LangString strRemove ${LANG_ITALIAN} "Vuoi Rimuovere le Impostazioni e gli eventi?"
LangString strNotRemoved ${LANG_ITALIAN} "Nota: $INSTDIR non è stato rimosso!"
LangString strStartMenuSec ${LANG_ITALIAN} "Scorciatoie Menu Avvio"
LangString strDesktopSec ${LANG_ITALIAN} "Scorciatoia Desktop"
LangString strStartupSec ${LANG_ITALIAN} "Esecuzione Automatica"
LangString strRainlendarDesc ${LANG_ITALIAN} "Installa il programma principale e skin e lingue predefinite."
LangString strStartMenuDesc ${LANG_ITALIAN} "Aggiungi Collegamenti nel Menu Avvio."
LangString strDesktopDesc ${LANG_ITALIAN} "Aggiungi Collegamento sul Desktop."
LangString strStartupDesc ${LANG_ITALIAN} "Aggiungi Collegamento in Esecuzione Automatica cosicché Rainlendar viene eseguito all'avvio di Windows."
LangString strClose ${LANG_ITALIAN} "Chiudere Rainlendar prima di Procedere."

LangString strRemove ${LANG_LATVIAN} "Vai vçlaties izmest uzstâdîjumus un notikumus?"
LangString strNotRemoved ${LANG_LATVIAN} "Piezîme: $INSTDIR nav iespçjams izdzçst!"
LangString strStartMenuSec ${LANG_LATVIAN} "Start izvçles norâde"
LangString strDesktopSec ${LANG_LATVIAN} "Darbgalda norâde"
LangString strStartupSec ${LANG_LATVIAN} "Uzsâkğanâs norâde"
LangString strRainlendarDesc ${LANG_LATVIAN} "Tiks instalçta pati programma un standarta apdares un valodas."
LangString strStartMenuDesc ${LANG_LATVIAN} "Tiks pievienota norâde jûsu Start izvçlei."
LangString strDesktopDesc ${LANG_LATVIAN} "Tiks pievienota norâde jûsu darbgaldam."
LangString strStartupDesc ${LANG_LATVIAN} "Tiks pievienota norâde uzsâkğanâs mapei, lai Rainlendar palaişas lîdz ar Windows."
LangString strClose ${LANG_LATVIAN} "Pirms jûs turpinât, lûdzu aizveriet Rainlendar."

LangString strRemove ${LANG_NORWEGIAN} "Ønsker du å fjærne innstillinger og hendelser?"
LangString strNotRemoved ${LANG_NORWEGIAN} "Notat: $INSTDIR kunne ikke fjernes!"
LangString strStartMenuSec ${LANG_NORWEGIAN} "Startmeny snarveier"
LangString strDesktopSec ${LANG_NORWEGIAN} "Skrivebords snarveier"
LangString strStartupSec ${LANG_NORWEGIAN} "Oppstarts snarvei"
LangString strRainlendarDesc ${LANG_NORWEGIAN} "Dette innstallerer hovedprogram og standard utseende & språk."
LangString strStartMenuDesc ${LANG_NORWEGIAN} "Dette legger snarveier i din Start Meny."
LangString strDesktopDesc ${LANG_NORWEGIAN} "Dette legger snarveier på ditt skrivebord."
LangString strStartupDesc ${LANG_NORWEGIAN} "Dette legger en snarvei i din oppstartsmappe slik at Rainlendar starter med Windows."
LangString strClose ${LANG_NORWEGIAN} "Venligst lukk Rainlendar før du fortsetter."

LangString strRemove ${LANG_PORTUGUESEBR} "Você deseja excluir as configurações e os eventos?"
LangString strNotRemoved ${LANG_PORTUGUESEBR} "Nota: $INSTDIR não pôde ser removido!"
LangString strStartMenuSec ${LANG_PORTUGUESEBR} "Atalhos do Menu Iniciar"
LangString strDesktopSec ${LANG_PORTUGUESEBR} "Atalho na Área de Trabalho"
LangString strStartupSec ${LANG_PORTUGUESEBR} "Atalho no Menu Inicializar"
LangString strRainlendarDesc ${LANG_PORTUGUESEBR} "Isso instala o programa principal e peles e idiomas padrões."
LangString strStartMenuDesc ${LANG_PORTUGUESEBR} "Isso adiciona os atalhos ao seu Menu Iniciar."
LangString strDesktopDesc ${LANG_PORTUGUESEBR} "Isso adiciona um atalho à sua Área de Trabalho."
LangString strStartupDesc ${LANG_PORTUGUESEBR} "Isso adiciona um atalho ao seu Menu Inicializar, fazendo com que o Rainlendar seja inicializado com o Windows."
LangString strClose ${LANG_PORTUGUESEBR} "Por favor feche o Rainlendar antes e prosseguir."

LangString strRemove ${LANG_SWEDISH} "Vill du ta bort alla inställningar och händelser?"
LangString strNotRemoved ${LANG_SWEDISH} "OBS! $INSTDIR kunde inte tas bort!"
LangString strStartMenuSec ${LANG_SWEDISH} "Genvägar i Start-menyn"
LangString strDesktopSec ${LANG_SWEDISH} "Genvägar på Skrivbordet"
LangString strStartupSec ${LANG_SWEDISH} "Genvägar i Autostart"
LangString strRainlendarDesc ${LANG_SWEDISH} "Installerar huvudprogrammet, standardskal och språk."
LangString strStartMenuDesc ${LANG_SWEDISH} "Skapar genvägar på Start-menyn."
LangString strDesktopDesc ${LANG_SWEDISH} "Skapar en genväg på Skrivbordet."
LangString strStartupDesc ${LANG_SWEDISH} "Skapar en genväg i Autostart så att Rainlendar startar med Windows."
LangString strClose ${LANG_SWEDISH} "Avsluta Rainlendar innan du fortsätter."



LangString strRemove ${LANG_ENGLISH} "Do you want to remove the settings and events?"
LangString strRemove ${LANG_FINNISH} "Haluatko poistaa myös asetukset ja tapahtumat?"
LangString strRemove ${LANG_POLISH} "Czy chcesz usun¹æ wydarzenia i ustawienia?"
LangString strRemove ${LANG_GERMAN} "Wollen Sie alle Einstellungen und Ereignisse entfernen?"
LangString strRemove ${LANG_FRENCH} "Souhaitez-vous supprimer le fichier de configuration ainsi que les événements ?"
LangString strRemove ${LANG_TRADCHINESE} "±z½T©w­n²¾°£©Ò¦³ªº³]©w»P¨Æ¥ó¶Ü?"
LangString strRemove ${LANG_SIMPCHINESE} "ÄúÏëÒª°ÑÉèÖÃºÍÊÂ¼şÒ»ÆğÒÆ³ıÃ´£¿"
LangString strRemove ${LANG_SLOVAK} "Chcete odstráni nastavenie a udalosti?"
LangString strRemove ${LANG_SPANISH} "¿Quiere eliminar la configuración y los eventos?"
LangString strRemove ${LANG_DANISH} "Ønsker du at fjerne opsætning og begivenheder?"
LangString strRemove ${LANG_DUTCH} "Wil je de instellingen en gebeurtenissen verwijderen?"
LangString strRemove ${LANG_CZECH} "Chcete odstranit nastavení a události?"
LangString strRemove ${LANG_RUSSIAN} "Âû õîòèòå óäàëèòü ôàéëû ñ íàñòğîéêàìè è ñîáûòèÿìè?"
LangString strRemove ${LANG_BULGARIAN} "Æåëàåòå ëè äà ïğåìàõíåòå íàñòğîéêèòå è ñúáèòèÿòà?"

LangString strNotRemoved ${LANG_ENGLISH} "Note: $INSTDIR could not be removed!"
LangString strNotRemoved ${LANG_FINNISH} "Huomio: $INSTDIR hakemistoa ei voitu poistaa!"
LangString strNotRemoved ${LANG_POLISH} "Uwaga: $INSTDIR nie móg³ zostaæ usuniêty!"
LangString strNotRemoved ${LANG_GERMAN} "Bemerkung: $INSTDIR konnte nicht entfernt werden!"
LangString strNotRemoved ${LANG_FRENCH} "Remarque : $INSTDIR n'a pas pu être supprimé !"
LangString strNotRemoved ${LANG_TRADCHINESE} "ª`·N: $INSTDIR µLªk³Q²¾°£!"
LangString strNotRemoved ${LANG_SIMPCHINESE} "×¢Òâ: $INSTDIR ÎŞ·¨±»ÒÆ³ı!"
LangString strNotRemoved ${LANG_SLOVAK} "Poznámka: $INSTDIR sa nedá zmaza!"
LangString strNotRemoved ${LANG_SPANISH} "Nota: $INSTDIR no se puede eliminar!"
LangString strNotRemoved ${LANG_DANISH} "Noter: $INSTDIR kunne ikke fjernes!"
LangString strNotRemoved ${LANG_DUTCH} "Opmerking: $INSTDIR Kon niet verwijderd worden!"
LangString strNotRemoved ${LANG_CZECH} "Poznámka: $INSTDIR nemùe bıt odstranìna!"
LangString strNotRemoved ${LANG_RUSSIAN} "Âíèìàíèå: ïàïêà $INSTDIR íå ìîæåò áûòü óäàëåíà!"
LangString strNotRemoved ${LANG_BULGARIAN} "Çàáåëåæêà: $INSTDIR Íå ìîæå äà áúäå ïğåìàõíàòà!"
                                

LangString strStartMenuSec ${LANG_ENGLISH} "Start menu shortcuts"
LangString strStartMenuSec ${LANG_FINNISH} "Käynnistä-valikon pikakuvakkeet"
LangString strStartMenuSec ${LANG_POLISH} "Skróty w Menu Start"
LangString strStartMenuSec ${LANG_GERMAN} "Startmenüverknüpfung"
LangString strStartMenuSec ${LANG_FRENCH} "Raccourcis dans le menu Démarrer"
LangString strStartMenuSec ${LANG_TRADCHINESE} "¶}©l¿ï³æ±¶®|"
LangString strStartMenuSec ${LANG_SIMPCHINESE} "¿ªÊ¼²Ëµ¥¿ì½İ·½Ê½"
LangString strStartMenuSec ${LANG_SLOVAK} "Zástupca v menu štart"
LangString strStartMenuSec ${LANG_SPANISH} "Acceso directo en Menú de Inicio"
LangString strStartMenuSec ${LANG_DANISH} "Start menu, genvej"
LangString strStartMenuSec ${LANG_DUTCH} "Start menu snelkoppeling"
LangString strStartMenuSec ${LANG_CZECH} "Zástupce v menu start"
LangString strStartMenuSec ${LANG_RUSSIAN} "Ãğóïïà â ìåíş Ïğîãğàììû"
LangString strStartMenuSec ${LANG_BULGARIAN} "Áúğçî ñòàğòèğàíå îò Start menu"


LangString strDesktopSec ${LANG_ENGLISH} "Desktop shortcut"
LangString strDesktopSec ${LANG_FINNISH} "Työpöytä pikakuvake"
LangString strDesktopSec ${LANG_POLISH} "Skróty na pulpicie"
LangString strDesktopSec ${LANG_GERMAN} "Desktopverknüpfung"
LangString strDesktopSec ${LANG_FRENCH} "Raccourci sur le bureau"
LangString strDesktopSec ${LANG_TRADCHINESE} "®à­±±¶®|"
LangString strDesktopSec ${LANG_SIMPCHINESE} "×ÀÃæ¿ì½İ·½Ê½"
LangString strDesktopSec ${LANG_SLOVAK} "Zástupca na ploche"
LangString strDesktopSec ${LANG_SPANISH} "Acceso directo en Escritorio"
LangString strDesktopSec ${LANG_DANISH} "Skrivebord, genvej"
LangString strDesktopSec ${LANG_DUTCH} "Bureaublad snelkoppeling"
LangString strDesktopSec ${LANG_CZECH} "Zástupce na ploše"
LangString strDesktopSec ${LANG_RUSSIAN} "ßğëûê íà ğàáî÷åì ñòîëå"
LangString strDesktopSec ${LANG_BULGARIAN} "Áúğçî ñòàğòèğàíå îò Desktop"


LangString strStartupSec ${LANG_ENGLISH} "Startup shortcut"
LangString strStartupSec ${LANG_FINNISH} "Käynnistä pikakuvake"
LangString strStartupSec ${LANG_POLISH} "Skróty w folderze Autostart"
LangString strStartupSec ${LANG_GERMAN} "Autostartverknüpfung"
LangString strStartupSec ${LANG_FRENCH} "Raccourci dans la barre de lancement rapide"
LangString strStartupSec ${LANG_TRADCHINESE} "±Ò°Ê±¶®|"
LangString strStartupSec ${LANG_SIMPCHINESE} "Æô¶¯À¸¿ì½İ·½Ê½"
LangString strStartupSec ${LANG_SLOVAK} "Spusti po štartu"
LangString strStartupSec ${LANG_SPANISH} "Acceso directo en Inicio"
LangString strStartupSec ${LANG_DANISH} "Opstarts genvej"
LangString strStartupSec ${LANG_DUTCH} "Opstart snelkoppeling"
LangString strStartupSec ${LANG_CZECH} "Spustit po startu"
LangString strStartupSec ${LANG_RUSSIAN} "ßğëûê â ïàïêå Àâòîçàãğóçêà"
LangString strStartupSec ${LANG_BULGARIAN} "Âğúçêà â Startup"


LangString strRainlendarDesc ${LANG_ENGLISH} "This installs the main program and the default skins & languages."
LangString strRainlendarDesc ${LANG_FINNISH} "Tämä asentaa pääohjelman, skinit sekä kielet."
LangString strRainlendarDesc ${LANG_POLISH} "Ta opcja instaluje g³ówny program oraz domyœlne skórki i pliki jêzykowe."
LangString strRainlendarDesc ${LANG_GERMAN} "Es wird das Hauptprogramm mit den Standard-Skins und -Sprachen installiert."
LangString strRainlendarDesc ${LANG_FRENCH} "Ceci installe le programme principal ainsi que les skins et langages par défaut."
LangString strRainlendarDesc ${LANG_TRADCHINESE} "³o±N·|¦w¸Ë¥Dµ{¦¡¥H¤Î¹w³]­±ª©»P»y¨t¡C"
LangString strRainlendarDesc ${LANG_SIMPCHINESE} "½«Òª°²×°Ö÷³ÌĞòºÍÈ±Ê¡µÄÆ¤·ôÒÔ¼°ÓïÑÔ."
LangString strRainlendarDesc ${LANG_SLOVAK} "Nainštaluje program, základné témy a jazyky."
LangString strRainlendarDesc ${LANG_SPANISH} "Instala el programa principal y los skins e idiomas por defecto."
LangString strRainlendarDesc ${LANG_DANISH} "Dette installere basisprogrammet med alle standard udseender og sprog."
LangString strRainlendarDesc ${LANG_DUTCH} "Dit installeert het hoofdprogramma en de standaard skins & talen."
LangString strRainlendarDesc ${LANG_CZECH} "Nainstaluje program, základní skiny a jazyky."
LangString strRainlendarDesc ${LANG_RUSSIAN} "Óñòàíîâêà ïğîãğàììû, îñíîâíûõ ñêèíîâ è ÿçûêîâûõ ôàéëîâ."
LangString strRainlendarDesc ${LANG_BULGARIAN} "Èíñòàëèğà îñíîâíàòà ïğîãğàìà è îñíîâíèòå îáâèâêèòå è åçèöè."


LangString strStartMenuDesc ${LANG_ENGLISH} "This adds the shortcuts to your Start Menu."
LangString strStartMenuDesc ${LANG_FINNISH} "Tämä lisää pikakuvakkeet Käynnistä-valikkoon."
LangString strStartMenuDesc ${LANG_POLISH} "Ta opcja dodaje skróty do Menu Start."
LangString strStartMenuDesc ${LANG_GERMAN} "Eine Verknüpfung wird im Startmenü hinzugefügt."
LangString strStartMenuDesc ${LANG_FRENCH} "Ceci ajoute des raccourcis à votre menu Démarrer."
LangString strStartMenuDesc ${LANG_TRADCHINESE} "³o±N·|·s¼W±¶®|©ó±zªº¶}©l¿ï³æ¤¤¡C"
LangString strStartMenuDesc ${LANG_SIMPCHINESE} "ÔÚÄãµÄ¿ªÊ¼²Ëµ¥ÖĞÌí¼Ó¿ì½İ·½Ê½."
LangString strStartMenuDesc ${LANG_SLOVAK} "Pridá zástupcu do menu Štart."
LangString strStartMenuDesc ${LANG_SPANISH} "Añade los accesos directos a su Menú de Inicio."
LangString strStartMenuDesc ${LANG_DANISH} "Dette tilføjer genvejen i Start Menuen.."
LangString strStartMenuDesc ${LANG_DUTCH} "Dit voegt een snelkoppeling toe aan het startmenu."
LangString strStartMenuDesc ${LANG_CZECH} "Pøidá zástupce do menu Start."
LangString strStartMenuDesc ${LANG_RUSSIAN} "Ñîçäàíèå ãğóïïû ÿğëûêîâ â ìåíş Ïğîãğàììû."
LangString strStartMenuDesc ${LANG_BULGARIAN} "Äîáàâÿ âğúçêà çà áúğçî ñòàğòèğàíå â Start Menu."


LangString strDesktopDesc ${LANG_ENGLISH} "This adds a shortcut to your desktop."
LangString strDesktopDesc ${LANG_FINNISH} "Tämä lisää pikakuvakkeen työpöydällesi."
LangString strDesktopDesc ${LANG_POLISH} "Ta opcja dodaje skróty na pulpit."
LangString strDesktopDesc ${LANG_GERMAN} "Eine Verknüpfung wird am Desktop hinzugefügt."
LangString strDesktopDesc ${LANG_FRENCH} "Ceci ajoute un raccourci sur votre bureau."
LangString strDesktopDesc ${LANG_TRADCHINESE} "³o±N·|·s¼W±¶®|©ó±zªº®à­±¤W¡C"
LangString strDesktopDesc ${LANG_SIMPCHINESE} "ÔÚÄãµÄ×ÀÃæÌí¼Ó¿ì½İ·½Ê½"
LangString strDesktopDesc ${LANG_SLOVAK} "Pridá zástupcu na plochu."
LangString strDesktopDesc ${LANG_SPANISH} "Añade un acceso directo a su Escritorio."
LangString strDesktopDesc ${LANG_DANISH} "Dette tilføjer genvejen på Skrivebordet."
LangString strDesktopDesc ${LANG_DUTCH} "Dit voegt een snelkoppeling toe uw bureaublad."
LangString strDesktopDesc ${LANG_CZECH} "Pøidá zástupce na plochu."
LangString strDesktopDesc ${LANG_RUSSIAN} "Ñîçäàíèå ÿğëûêà íà ğàáî÷åì ñòîëå."
LangString strDesktopDesc ${LANG_BULGARIAN} "Äîáàâÿ âğúçêà çà áúğçî ñòàğòèğàíå íà Desktop."


LangString strStartupDesc ${LANG_ENGLISH} "This adds a shortcut to your startup folder so that Rainlendar start with Windows."
LangString strStartupDesc ${LANG_FINNISH} "Tämä lisää pikakuvakeen Aloitus-hakemistoon, jotta Rainlendar käynnistyy Windowsin kanssa."
LangString strStartupDesc ${LANG_POLISH} "Ta opcja dodaje skróty do folderu Autostart, ¿eby Rainlendar by³ uruchamiany przy starcie Windows."
LangString strStartupDesc ${LANG_GERMAN} "Eine Verknüpfung wird im Startmenü hinzugefügt und Rainlendar wird automatisch mit Windows gestartet."
LangString strStartupDesc ${LANG_FRENCH} "Ceci ajoute un raccourci dans votre dossier de démarrage, afin que Rainlendar démarre avec Windows."
LangString strStartupDesc ${LANG_TRADCHINESE} "³o±N·|·s¼W±¶®|©ó±zªº±Ò°Ê¥Ø¿ı¤¤¡A©Ò¥H Rainlendar ±N·|¦ñÀH Windows ¤@°_±Ò°Ê¡C"
LangString strStartupDesc ${LANG_SIMPCHINESE} "ÔÚWindows¿ªÊ¼Ê±Ò»ÆğÆô¶¯."
LangString strStartupDesc ${LANG_SLOVAK} "Spusti pri štartu - umiestní zástupcu do záloky po spustení."
LangString strStartupDesc ${LANG_SPANISH} "Añade un acceso directo a su carpeta de Inicio de forma que Rainlendar se inicie con Windows."
LangString strStartupDesc ${LANG_DANISH} "Dette tilføjer genvejen til din Startmappe så Rainlendar starter sammen med Windows."
LangString strStartupDesc ${LANG_DUTCH} "Dit voegt een snelkoppeling toe aan uw opstartmenu zodat deze direct start met Windows."
LangString strStartupDesc ${LANG_CZECH} "Spustit pøi startu - umístí zástupce do sloky po spuštìní."
LangString strStartupDesc ${LANG_RUSSIAN} "Ñîçäàíèå ÿğëûêà â ïàïêå Àâòîçàãğóçêà äëÿ àâòîìàòè÷åñêîãî çàïóñêà Rainlendar âìåñòå ñ Windows."
LangString strStartupDesc ${LANG_BULGARIAN} "Äîáàâÿ âğúçêà âúâ Âàøàòà Startup äèğåêòîğèÿ òàêà, ÷å Rainlendar äà ñå ñòàğòèğà çàåäíî ñ Windows."


LangString strClose ${LANG_ENGLISH} "Please close Rainlendar before you proceed."
LangString strClose ${LANG_FINNISH} "Sulje Rainlendar ennen kuin jatkat."
LangString strClose ${LANG_POLISH} "Proszê zamkn¹æ Rainlendar zanim bêdziesz kontynuowaæ."
LangString strClose ${LANG_GERMAN} "Bitte Beenden Sie Rainlendar bevor Sie fortfahren."
LangString strClose ${LANG_FRENCH} "Merci de fermer Rainlendar avant de continuer."
LangString strClose ${LANG_TRADCHINESE} "¦b°õ¦æ¦¹µ{¦¡«e½Ğ¥ıÃö³¬ Rainlendar ¡C"
LangString strClose ${LANG_SIMPCHINESE} "½øĞĞÇëÄúÏÈ¹Ø±ÕRainlendar."
LangString strClose ${LANG_SLOVAK} "Ukonèite Rainlendar skôr ne budete pokraèova."
LangString strClose ${LANG_SPANISH} "Por favor, cierre Rainlendar antes de proceder."
LangString strClose ${LANG_DANISH} "Vær venlig at lukke Rainlendar før du fortsætter."
LangString strClose ${LANG_DUTCH} "Sluit Rainlendar af voor u verder gaat."
LangString strClose ${LANG_CZECH} "Ukonèete Rainlendar ne budete pokraèovat."
LangString strClose ${LANG_RUSSIAN} "Ïîæàëóéñòà, çàêğîéòå Rainlendar è íàæìèòå êíîïêó Ïîâòîğèòü."
LangString strClose ${LANG_BULGARIAN} "Ìîëÿ çàòâîğåòå Rainlendar ïğåäè äà ïğîäúëæèòå."


#
# Install
#
Section "Rainlendar" secRainlendar ; (default section)
  SetOutPath "$INSTDIR"
  SectionIn 1 RO

RainlendarCheckAgain:
  FindWindow $0 "Rainlendar"
  IsWindow $0 0 RainlendarDoesntExistLabel
    MessageBox MB_ABORTRETRYIGNORE|MB_ICONSTOP $(strClose) IDRETRY RainlendarCheckAgain IDIGNORE RainlendarDoesntExistLabel
    Abort
    
RainlendarDoesntExistLabel:

  File /r "Distrib\*.*"
  
  WriteRegStr HKEY_LOCAL_MACHINE "SOFTWARE\Rainlendar" "" "$INSTDIR"
  WriteRegStr HKEY_LOCAL_MACHINE "Software\Microsoft\Windows\CurrentVersion\Uninstall\Rainlendar" "DisplayName" "Rainlendar (remove only)"
  WriteRegStr HKEY_LOCAL_MACHINE "Software\Microsoft\Windows\CurrentVersion\Uninstall\Rainlendar" "UninstallString" '"$INSTDIR\uninst.exe"'
  
  WriteUninstaller "$INSTDIR\uninst.exe"

SectionEnd ; end of default section

Section $(strStartMenuSec) secStartMenu
  SectionIn 1

  # Shortcut on Start Menu (Directory and Uninstall)
  CreateDirectory "$SMPROGRAMS\Rainlendar"
  CreateShortCut  "$SMPROGRAMS\Rainlendar\Rainlendar.lnk" "$INSTDIR\rainlendar.exe" "" "$INSTDIR\rainlendar.exe" 0
  CreateShortCut  "$SMPROGRAMS\Rainlendar\Documentation.lnk" "$INSTDIR\rainlendar.chm" "" "$INSTDIR\rainlendar.chm" 0
  CreateShortCut  "$SMPROGRAMS\Rainlendar\Remove Rainlendar.lnk" "$INSTDIR\uninst.exe" "" "$INSTDIR\uninst.exe" 0
SectionEnd

Section $(strDesktopSec) secDesktop
  SectionIn 1
  CreateShortCut  "$DESKTOP\Rainlendar.lnk" "$INSTDIR\rainlendar.exe" "" "$INSTDIR\rainlendar.exe" 0
SectionEnd

Section $(strStartupSec) secStartup
  SectionIn 1
  CreateShortCut  "$SMSTARTUP\Rainlendar.lnk" "$INSTDIR\rainlendar.exe" "" "$INSTDIR\rainlendar.exe" 0
SectionEnd

!insertmacro MUI_FUNCTION_DESCRIPTION_BEGIN
  !insertmacro MUI_DESCRIPTION_TEXT ${secRainlendar} $(strRainlendarDesc)
  !insertmacro MUI_DESCRIPTION_TEXT ${secStartMenu} $(strStartMenuDesc)
  !insertmacro MUI_DESCRIPTION_TEXT ${secDesktop} $(strDesktopDesc)
  !insertmacro MUI_DESCRIPTION_TEXT ${secStartup} $(strStartupDesc)
!insertmacro MUI_FUNCTION_DESCRIPTION_END

#
# Uninstall
#
Section Uninstall

RainlendarCheckAgain2:
  FindWindow $0 "Rainlendar"
  IsWindow $0 0 RainlendarDoesntExistLabel2
    MessageBox MB_ABORTRETRYIGNORE|MB_ICONSTOP $(strClose) IDRETRY RainlendarCheckAgain2 IDIGNORE RainlendarDoesntExistLabel2
    Abort

RainlendarDoesntExistLabel2:

# Check for settings
  IfFileExists "$APPDATA\Rainlendar\*.*" 0 LeaveSettingsLabel
  
  MessageBox MB_YESNO|MB_ICONQUESTION $(strRemove) IDNO LeaveSettingsLabel

  Delete "$APPDATA\Rainlendar\*.*"
  RMDir "$APPDATA\Rainlendar"

LeaveSettingsLabel:

  Delete "$INSTDIR\Skins\Tranquil\*.*"
  Delete "$INSTDIR\Skins\Shadow3.1\Icons\*.*"
  Delete "$INSTDIR\Skins\Shadow3.1\*.*"
  Delete "$INSTDIR\Languages\English.example"
  Delete "$INSTDIR\Languages\Finnish.ini"
  Delete "$INSTDIR\Plugins\*.*"
  Delete "$INSTDIR\*.*"
  RMDir "$INSTDIR\Skins\Tranquil"
  RMDir "$INSTDIR\Skins\Shadow3.1\Icons"
  RMDir "$INSTDIR\Skins\Shadow3.1"
  RMDir "$INSTDIR\Languages"
  RMDir "$INSTDIR\Skins"
  RMDir "$INSTDIR\Plugins"
  DeleteRegKey HKLM SOFTWARE\Rainlendar
  DeleteRegKey HKLM "SOFTWARE\Microsoft\Windows\CurrentVersion\Uninstall\Rainlendar"
  RMDir "$INSTDIR"
  Delete "$SMSTARTUP\Rainlendar.lnk"
  Delete "$DESKTOP\Rainlendar.lnk"
  Delete "$SMPROGRAMS\Rainlendar\*.*"
  RMDir "$SMPROGRAMS\Rainlendar"

  IfFileExists "$INSTDIR" 0 NoErrorMsg
    MessageBox MB_OK $(strNotRemoved) IDOK 0 ; skipped if file doesn't exist
  NoErrorMsg:
  
SectionEnd

Function .onInit
  !insertmacro MUI_LANGDLL_DISPLAY
FunctionEnd

Function un.onInit
  !insertmacro MUI_LANGDLL_DISPLAY
FunctionEnd

; eof

