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

LangString strRemove ${LANG_HUNGARIAN}         "El akarja t�vol�tani a be�ll�t�sokat �s az esem�nyeket?"                                    
LangString strNotRemoved ${LANG_HUNGARIAN}     "Megjegyz�s: $INSTDIR nem lehet elt�vol�tani!"                                               
LangString strStartMenuSec ${LANG_HUNGARIAN}   "Start men� parancsikon"                                                                     
LangString strDesktopSec ${LANG_HUNGARIAN}     "Asztal parancsikon"                                                                         
LangString strStartupSec ${LANG_HUNGARIAN}     "Ind�t�pult parancsikon"                                                                     
LangString strRainlendarDesc ${LANG_HUNGARIAN} "Ez telep�ti a programot, az alap b�r�ket �s a nyelvi f�jlokat."                             
LangString strStartMenuDesc ${LANG_HUNGARIAN}  "Ez hozz�adja a parancsikont a Start men�h�z."                                               
LangString strDesktopDesc ${LANG_HUNGARIAN}    "Ez hozz�adja a parancsikont az Asztalhoz."                                                  
LangString strStartupDesc ${LANG_HUNGARIAN}    "Ez hozz�adja a parancsikont az Ind�t�pulthoz, �gy a Rainlendar a Windows-al indul."         
LangString strClose ${LANG_HUNGARIAN}          "K�rem z�rja be a Rainlendar-t, miel�tt folytatn�."                                          

LangString strRemove ${LANG_GREEK} "������ �� ���������� ��� ��������� ��� �� ��������;"
LangString strNotRemoved ${LANG_GREEK} "��������: $INSTDIR ��� ������ �� ���������!"
LangString strStartMenuSec ${LANG_GREEK} "������������ ����� ���������"
LangString strDesktopSec ${LANG_GREEK} "������������ ���������� ��������"
LangString strStartupSec ${LANG_GREEK} "���������� ���������"
LangString strRainlendarDesc ${LANG_GREEK} "��������� �� ����� ���������, �� �������������� ������ ��� �������."
LangString strStartMenuDesc ${LANG_GREEK} "��������� ��� ������������ ��� ����� ���������."
LangString strDesktopDesc ${LANG_GREEK} "��������� ���������� ���� ��������� ��������."
LangString strStartupDesc ${LANG_GREEK} "��������� ���������� ���� ������ ��������� ���� ���� �� Rainlendar �� ������ �� �� Windows."
LangString strClose ${LANG_GREEK} "�������� ������ �� Rainlendar ���� ����������."

LangString strRemove ${LANG_KOREAN} "������ �̺�Ʈ ������� �����Ͻðڽ��ϱ�?"
LangString strNotRemoved ${LANG_KOREAN} "�˸�: $INSTDIR �� �������� �ʾҽ��ϴ�!"
LangString strStartMenuSec ${LANG_KOREAN} "���۸޴��� �ٷΰ���"
LangString strDesktopSec ${LANG_KOREAN} "����ȭ���� �ٷΰ���"
LangString strStartupSec ${LANG_KOREAN} "�������α׷��� �ٷΰ���(������ ���۽� �ڵ�����)"
LangString strRainlendarDesc ${LANG_KOREAN} "��ġ���α׷��� �⺻ ���� ��Ų�� ����մϴ�."
LangString strStartMenuDesc ${LANG_KOREAN} "���۸޴��� �ٷΰ��⸦ �߰��մϴ�."
LangString strDesktopDesc ${LANG_KOREAN} "����ȭ�鿡 �ٷΰ��⸦ �߰��մϴ�."
LangString strStartupDesc ${LANG_KOREAN} "�������α׷��� �ٷΰ��⸦ �߰��ؼ� �����찡 ���۵� ������ ����ǵ��� �մϴ�."
LangString strClose ${LANG_KOREAN} "�̹� Rainlendar�� �������̶�� ��ġ�� ���� ������ �ֽʽÿ�."

LangString strRemove ${LANG_ALBANIAN} "Ju d�shironi t� suprimoni konfigurimin e po ashtu ngjarjet ?"
LangString strNotRemoved ${LANG_ALBANIAN} "V�rejtje : $INSTDIR nuk ka mundur t� fshihet!"
LangString strStartMenuSec ${LANG_ALBANIAN} "Shkurtore n� menyn� Start"
LangString strDesktopSec ${LANG_ALBANIAN} "Shkurtore n� Byro"
LangString strStartupSec ${LANG_ALBANIAN} "Shkurtore n� startim"
LangString strRainlendarDesc ${LANG_ALBANIAN} "Kjo instalon programin kresor gjithashtu pamjen dhe gjuh�n e parazgjedhur."
LangString strStartMenuDesc ${LANG_ALBANIAN} "Kjo shton shkurtore n� menyn� Start."
LangString strDesktopDesc ${LANG_ALBANIAN} "Kjo shton shkurtore n� Byro."
LangString strStartupDesc ${LANG_ALBANIAN} "Kjo shton shkurtore n� dosjen tuaj t� startimit, n� menyr� q� Rainlendar t� startoj me Windows."
LangString strClose ${LANG_ALBANIAN} "Ju lutem mbylleni Rainlendar para se te vazhdoni."

LangString strRemove ${LANG_CATALAN} "Vols esborrar les configuracions i les notes?"
LangString strNotRemoved ${LANG_CATALAN} "Nota: &INSTDIR no pot ser eliminat!"
LangString strStartMenuSec ${LANG_CATALAN} "Drecera al men� d'inici"
LangString strDesktopSec ${LANG_CATALAN} "Drecera a l'escriptori"
LangString strStartupSec ${LANG_CATALAN} "Drecera al iniciar"
LangString strRainlendarDesc ${LANG_CATALAN} "Aix� instal�la el programa principal i les aparences & lleng�es per defecte"
LangString strStartMenuDesc ${LANG_CATALAN} "Aix� afegeix les dreceres al men� d'inici"
LangString strDesktopDesc ${LANG_CATALAN} "Aix� afegeix la drecera a l'escriptori"
LangString strStartupDesc ${LANG_CATALAN} "Aix� afegeix la drecera a la teva carpeta d'inici i per tant Rainlendar s'iniciar� amb Windows."
LangString strClose ${LANG_CATALAN} "Si us plau, tanca el Rainlendar abans de continuar."

LangString strRemove ${LANG_CROATIAN} "�elite li maknuti postavke i doga�aje?"
LangString strNotRemoved ${LANG_CROATIAN} "Napomena: $INSTDIR se nije mogao maknuti!"
LangString strStartMenuSec ${LANG_CROATIAN} "Kratice u Start izborniku"
LangString strDesktopSec ${LANG_CROATIAN} "Desktop kratice"
LangString strStartupSec ${LANG_CROATIAN} "Startup kratice"
LangString strRainlendarDesc ${LANG_CROATIAN} "Ovo instalira glavni program i uobi�ajene izglede i jezike."
LangString strStartMenuDesc ${LANG_CROATIAN} "Ovo dodaje kratice u va� Start izbornik."
LangString strDesktopDesc ${LANG_CROATIAN} "Ovo dodaje kratice na va� Desktop."
LangString strStartupDesc ${LANG_CROATIAN} "Ovo dodaje kratice u va� Startup direktorij da se Rainlendar mo�e pokrenuti s Windowsima."
LangString strClose ${LANG_CROATIAN} "Molimo, zatvorite Rainlendar prije nego �to nastavite."

LangString strRemove ${LANG_ESTONIAN} "Kas tahad kustutada ka s�tted ja s�ndmused?"
LangString strNotRemoved ${LANG_ESTONIAN} "NB! $INSTDIR kataloogi ei saanud kustutada!"
LangString strStartMenuSec ${LANG_ESTONIAN} "Start men�� otsetee"
LangString strDesktopSec ${LANG_ESTONIAN} "Desktop otsetee"
LangString strStartupSec ${LANG_ESTONIAN} "K�ivita otsetee"
LangString strRainlendarDesc ${LANG_ESTONIAN} "See installeerib p�hiprogrammi, nahad ja keeled"
LangString strStartMenuDesc ${LANG_ESTONIAN} "See lisab otseteed Start men��sse"
LangString strDesktopDesc ${LANG_ESTONIAN} "See lisab otseteed Start Desktopile"
LangString strStartupDesc ${LANG_ESTONIAN} "See lisab otseteed Startup kataloogi, et Rainlendar k�ivituks koos Windowsi'ga"
LangString strClose ${LANG_ESTONIAN} "Sulge Rainlendar enne kui j�tkad!"

LangString strRemove ${LANG_HEBREW} "?��� ��� ���� ����� �� ������� ���������"
LangString strNotRemoved ${LANG_HEBREW} "!��� ��: �� ���� ����� �� ������ ������"
LangString strStartMenuSec ${LANG_HEBREW} "������ ��� ������ ����"
LangString strDesktopSec ${LANG_HEBREW} "����� ��� ������ ������"
LangString strStartupSec ${LANG_HEBREW} "����� ��� ������"
LangString strRainlendarDesc ${LANG_HEBREW} "����� �� ������ ������ ��� �� ����� �������� ����� �����"
LangString strStartMenuDesc ${LANG_HEBREW} "����� ������ ��� ������ ����"
LangString strDesktopDesc ${LANG_HEBREW} "����� ����� ��� ������ ������ ���"
LangString strStartupDesc ${LANG_HEBREW} "����� ����� ��� ������� ������ �� ������� ����� ��� �� ������"
LangString strClose ${LANG_HEBREW} "��� ���� �� ������ ���� ������"

LangString strRemove ${LANG_ITALIAN} "Vuoi Rimuovere le Impostazioni e gli eventi?"
LangString strNotRemoved ${LANG_ITALIAN} "Nota: $INSTDIR non � stato rimosso!"
LangString strStartMenuSec ${LANG_ITALIAN} "Scorciatoie Menu Avvio"
LangString strDesktopSec ${LANG_ITALIAN} "Scorciatoia Desktop"
LangString strStartupSec ${LANG_ITALIAN} "Esecuzione Automatica"
LangString strRainlendarDesc ${LANG_ITALIAN} "Installa il programma principale e skin e lingue predefinite."
LangString strStartMenuDesc ${LANG_ITALIAN} "Aggiungi Collegamenti nel Menu Avvio."
LangString strDesktopDesc ${LANG_ITALIAN} "Aggiungi Collegamento sul Desktop."
LangString strStartupDesc ${LANG_ITALIAN} "Aggiungi Collegamento in Esecuzione Automatica cosicch� Rainlendar viene eseguito all'avvio di Windows."
LangString strClose ${LANG_ITALIAN} "Chiudere Rainlendar prima di Procedere."

LangString strRemove ${LANG_LATVIAN} "Vai v�laties izmest uzst�d�jumus un notikumus?"
LangString strNotRemoved ${LANG_LATVIAN} "Piez�me: $INSTDIR nav iesp�jams izdz�st!"
LangString strStartMenuSec ${LANG_LATVIAN} "Start izv�les nor�de"
LangString strDesktopSec ${LANG_LATVIAN} "Darbgalda nor�de"
LangString strStartupSec ${LANG_LATVIAN} "Uzs�k�an�s nor�de"
LangString strRainlendarDesc ${LANG_LATVIAN} "Tiks instal�ta pati programma un standarta apdares un valodas."
LangString strStartMenuDesc ${LANG_LATVIAN} "Tiks pievienota nor�de j�su Start izv�lei."
LangString strDesktopDesc ${LANG_LATVIAN} "Tiks pievienota nor�de j�su darbgaldam."
LangString strStartupDesc ${LANG_LATVIAN} "Tiks pievienota nor�de uzs�k�an�s mapei, lai Rainlendar palai�as l�dz ar Windows."
LangString strClose ${LANG_LATVIAN} "Pirms j�s turpin�t, l�dzu aizveriet Rainlendar."

LangString strRemove ${LANG_NORWEGIAN} "�nsker du � fj�rne innstillinger og hendelser?"
LangString strNotRemoved ${LANG_NORWEGIAN} "Notat: $INSTDIR kunne ikke fjernes!"
LangString strStartMenuSec ${LANG_NORWEGIAN} "Startmeny snarveier"
LangString strDesktopSec ${LANG_NORWEGIAN} "Skrivebords snarveier"
LangString strStartupSec ${LANG_NORWEGIAN} "Oppstarts snarvei"
LangString strRainlendarDesc ${LANG_NORWEGIAN} "Dette innstallerer hovedprogram og standard utseende & spr�k."
LangString strStartMenuDesc ${LANG_NORWEGIAN} "Dette legger snarveier i din Start Meny."
LangString strDesktopDesc ${LANG_NORWEGIAN} "Dette legger snarveier p� ditt skrivebord."
LangString strStartupDesc ${LANG_NORWEGIAN} "Dette legger en snarvei i din oppstartsmappe slik at Rainlendar starter med Windows."
LangString strClose ${LANG_NORWEGIAN} "Venligst lukk Rainlendar f�r du fortsetter."

LangString strRemove ${LANG_PORTUGUESEBR} "Voc� deseja excluir as configura��es e os eventos?"
LangString strNotRemoved ${LANG_PORTUGUESEBR} "Nota: $INSTDIR n�o p�de ser removido!"
LangString strStartMenuSec ${LANG_PORTUGUESEBR} "Atalhos do Menu Iniciar"
LangString strDesktopSec ${LANG_PORTUGUESEBR} "Atalho na �rea de Trabalho"
LangString strStartupSec ${LANG_PORTUGUESEBR} "Atalho no Menu Inicializar"
LangString strRainlendarDesc ${LANG_PORTUGUESEBR} "Isso instala o programa principal e peles e idiomas padr�es."
LangString strStartMenuDesc ${LANG_PORTUGUESEBR} "Isso adiciona os atalhos ao seu Menu Iniciar."
LangString strDesktopDesc ${LANG_PORTUGUESEBR} "Isso adiciona um atalho � sua �rea de Trabalho."
LangString strStartupDesc ${LANG_PORTUGUESEBR} "Isso adiciona um atalho ao seu Menu Inicializar, fazendo com que o Rainlendar seja inicializado com o Windows."
LangString strClose ${LANG_PORTUGUESEBR} "Por favor feche o Rainlendar antes e prosseguir."

LangString strRemove ${LANG_SWEDISH} "Vill du ta bort alla inst�llningar och h�ndelser?"
LangString strNotRemoved ${LANG_SWEDISH} "OBS! $INSTDIR kunde inte tas bort!"
LangString strStartMenuSec ${LANG_SWEDISH} "Genv�gar i Start-menyn"
LangString strDesktopSec ${LANG_SWEDISH} "Genv�gar p� Skrivbordet"
LangString strStartupSec ${LANG_SWEDISH} "Genv�gar i Autostart"
LangString strRainlendarDesc ${LANG_SWEDISH} "Installerar huvudprogrammet, standardskal och spr�k."
LangString strStartMenuDesc ${LANG_SWEDISH} "Skapar genv�gar p� Start-menyn."
LangString strDesktopDesc ${LANG_SWEDISH} "Skapar en genv�g p� Skrivbordet."
LangString strStartupDesc ${LANG_SWEDISH} "Skapar en genv�g i Autostart s� att Rainlendar startar med Windows."
LangString strClose ${LANG_SWEDISH} "Avsluta Rainlendar innan du forts�tter."



LangString strRemove ${LANG_ENGLISH} "Do you want to remove the settings and events?"
LangString strRemove ${LANG_FINNISH} "Haluatko poistaa my�s asetukset ja tapahtumat?"
LangString strRemove ${LANG_POLISH} "Czy chcesz usun�� wydarzenia i ustawienia?"
LangString strRemove ${LANG_GERMAN} "Wollen Sie alle Einstellungen und Ereignisse entfernen?"
LangString strRemove ${LANG_FRENCH} "Souhaitez-vous supprimer le fichier de configuration ainsi que les �v�nements ?"
LangString strRemove ${LANG_TRADCHINESE} "�z�T�w�n�����Ҧ����]�w�P�ƥ��?"
LangString strRemove ${LANG_SIMPCHINESE} "����Ҫ�����ú��¼�һ���Ƴ�ô��"
LangString strRemove ${LANG_SLOVAK} "Chcete odstr�ni� nastavenie a udalosti?"
LangString strRemove ${LANG_SPANISH} "�Quiere eliminar la configuraci�n y los eventos?"
LangString strRemove ${LANG_DANISH} "�nsker du at fjerne ops�tning og begivenheder?"
LangString strRemove ${LANG_DUTCH} "Wil je de instellingen en gebeurtenissen verwijderen?"
LangString strRemove ${LANG_CZECH} "Chcete odstranit nastaven� a ud�losti?"
LangString strRemove ${LANG_RUSSIAN} "�� ������ ������� ����� � ����������� � ���������?"
LangString strRemove ${LANG_BULGARIAN} "������� �� �� ���������� ����������� � ���������?"

LangString strNotRemoved ${LANG_ENGLISH} "Note: $INSTDIR could not be removed!"
LangString strNotRemoved ${LANG_FINNISH} "Huomio: $INSTDIR hakemistoa ei voitu poistaa!"
LangString strNotRemoved ${LANG_POLISH} "Uwaga: $INSTDIR nie m�g� zosta� usuni�ty!"
LangString strNotRemoved ${LANG_GERMAN} "Bemerkung: $INSTDIR konnte nicht entfernt werden!"
LangString strNotRemoved ${LANG_FRENCH} "Remarque : $INSTDIR n'a pas pu �tre supprim� !"
LangString strNotRemoved ${LANG_TRADCHINESE} "�`�N: $INSTDIR �L�k�Q����!"
LangString strNotRemoved ${LANG_SIMPCHINESE} "ע��: $INSTDIR �޷����Ƴ�!"
LangString strNotRemoved ${LANG_SLOVAK} "Pozn�mka: $INSTDIR sa ned� zmaza�!"
LangString strNotRemoved ${LANG_SPANISH} "Nota: $INSTDIR no se puede eliminar!"
LangString strNotRemoved ${LANG_DANISH} "Noter: $INSTDIR kunne ikke fjernes!"
LangString strNotRemoved ${LANG_DUTCH} "Opmerking: $INSTDIR Kon niet verwijderd worden!"
LangString strNotRemoved ${LANG_CZECH} "Pozn�mka: $INSTDIR nem��e b�t odstran�na!"
LangString strNotRemoved ${LANG_RUSSIAN} "��������: ����� $INSTDIR �� ����� ���� �������!"
LangString strNotRemoved ${LANG_BULGARIAN} "���������: $INSTDIR �� ���� �� ���� ����������!"
                                

LangString strStartMenuSec ${LANG_ENGLISH} "Start menu shortcuts"
LangString strStartMenuSec ${LANG_FINNISH} "K�ynnist�-valikon pikakuvakkeet"
LangString strStartMenuSec ${LANG_POLISH} "Skr�ty w Menu Start"
LangString strStartMenuSec ${LANG_GERMAN} "Startmen�verkn�pfung"
LangString strStartMenuSec ${LANG_FRENCH} "Raccourcis dans le menu D�marrer"
LangString strStartMenuSec ${LANG_TRADCHINESE} "�}�l��汶�|"
LangString strStartMenuSec ${LANG_SIMPCHINESE} "��ʼ�˵���ݷ�ʽ"
LangString strStartMenuSec ${LANG_SLOVAK} "Z�stupca v menu �tart"
LangString strStartMenuSec ${LANG_SPANISH} "Acceso directo en Men� de Inicio"
LangString strStartMenuSec ${LANG_DANISH} "Start menu, genvej"
LangString strStartMenuSec ${LANG_DUTCH} "Start menu snelkoppeling"
LangString strStartMenuSec ${LANG_CZECH} "Z�stupce v menu start"
LangString strStartMenuSec ${LANG_RUSSIAN} "������ � ���� ���������"
LangString strStartMenuSec ${LANG_BULGARIAN} "����� ���������� �� Start menu"


LangString strDesktopSec ${LANG_ENGLISH} "Desktop shortcut"
LangString strDesktopSec ${LANG_FINNISH} "Ty�p�yt� pikakuvake"
LangString strDesktopSec ${LANG_POLISH} "Skr�ty na pulpicie"
LangString strDesktopSec ${LANG_GERMAN} "Desktopverkn�pfung"
LangString strDesktopSec ${LANG_FRENCH} "Raccourci sur le bureau"
LangString strDesktopSec ${LANG_TRADCHINESE} "�ୱ���|"
LangString strDesktopSec ${LANG_SIMPCHINESE} "�����ݷ�ʽ"
LangString strDesktopSec ${LANG_SLOVAK} "Z�stupca na ploche"
LangString strDesktopSec ${LANG_SPANISH} "Acceso directo en Escritorio"
LangString strDesktopSec ${LANG_DANISH} "Skrivebord, genvej"
LangString strDesktopSec ${LANG_DUTCH} "Bureaublad snelkoppeling"
LangString strDesktopSec ${LANG_CZECH} "Z�stupce na plo�e"
LangString strDesktopSec ${LANG_RUSSIAN} "����� �� ������� �����"
LangString strDesktopSec ${LANG_BULGARIAN} "����� ���������� �� Desktop"


LangString strStartupSec ${LANG_ENGLISH} "Startup shortcut"
LangString strStartupSec ${LANG_FINNISH} "K�ynnist� pikakuvake"
LangString strStartupSec ${LANG_POLISH} "Skr�ty w folderze Autostart"
LangString strStartupSec ${LANG_GERMAN} "Autostartverkn�pfung"
LangString strStartupSec ${LANG_FRENCH} "Raccourci dans la barre de lancement rapide"
LangString strStartupSec ${LANG_TRADCHINESE} "�Ұʱ��|"
LangString strStartupSec ${LANG_SIMPCHINESE} "��������ݷ�ʽ"
LangString strStartupSec ${LANG_SLOVAK} "Spusti� po �tartu"
LangString strStartupSec ${LANG_SPANISH} "Acceso directo en Inicio"
LangString strStartupSec ${LANG_DANISH} "Opstarts genvej"
LangString strStartupSec ${LANG_DUTCH} "Opstart snelkoppeling"
LangString strStartupSec ${LANG_CZECH} "Spustit po startu"
LangString strStartupSec ${LANG_RUSSIAN} "����� � ����� ������������"
LangString strStartupSec ${LANG_BULGARIAN} "������ � Startup"


LangString strRainlendarDesc ${LANG_ENGLISH} "This installs the main program and the default skins & languages."
LangString strRainlendarDesc ${LANG_FINNISH} "T�m� asentaa p��ohjelman, skinit sek� kielet."
LangString strRainlendarDesc ${LANG_POLISH} "Ta opcja instaluje g��wny program oraz domy�lne sk�rki i pliki j�zykowe."
LangString strRainlendarDesc ${LANG_GERMAN} "Es wird das Hauptprogramm mit den Standard-Skins und -Sprachen installiert."
LangString strRainlendarDesc ${LANG_FRENCH} "Ceci installe le programme principal ainsi que les skins et langages par d�faut."
LangString strRainlendarDesc ${LANG_TRADCHINESE} "�o�N�|�w�˥D�{���H�ιw�]�����P�y�t�C"
LangString strRainlendarDesc ${LANG_SIMPCHINESE} "��Ҫ��װ�������ȱʡ��Ƥ���Լ�����."
LangString strRainlendarDesc ${LANG_SLOVAK} "Nain�taluje program, z�kladn� t�my a jazyky."
LangString strRainlendarDesc ${LANG_SPANISH} "Instala el programa principal y los skins e idiomas por defecto."
LangString strRainlendarDesc ${LANG_DANISH} "Dette installere basisprogrammet med alle standard udseender og sprog."
LangString strRainlendarDesc ${LANG_DUTCH} "Dit installeert het hoofdprogramma en de standaard skins & talen."
LangString strRainlendarDesc ${LANG_CZECH} "Nainstaluje program, z�kladn� skiny a jazyky."
LangString strRainlendarDesc ${LANG_RUSSIAN} "��������� ���������, �������� ������ � �������� ������."
LangString strRainlendarDesc ${LANG_BULGARIAN} "��������� ��������� �������� � ��������� ��������� � �����."


LangString strStartMenuDesc ${LANG_ENGLISH} "This adds the shortcuts to your Start Menu."
LangString strStartMenuDesc ${LANG_FINNISH} "T�m� lis�� pikakuvakkeet K�ynnist�-valikkoon."
LangString strStartMenuDesc ${LANG_POLISH} "Ta opcja dodaje skr�ty do Menu Start."
LangString strStartMenuDesc ${LANG_GERMAN} "Eine Verkn�pfung wird im Startmen� hinzugef�gt."
LangString strStartMenuDesc ${LANG_FRENCH} "Ceci ajoute des raccourcis � votre menu D�marrer."
LangString strStartMenuDesc ${LANG_TRADCHINESE} "�o�N�|�s�W���|��z���}�l��椤�C"
LangString strStartMenuDesc ${LANG_SIMPCHINESE} "����Ŀ�ʼ�˵�����ӿ�ݷ�ʽ."
LangString strStartMenuDesc ${LANG_SLOVAK} "Prid� z�stupcu do menu �tart."
LangString strStartMenuDesc ${LANG_SPANISH} "A�ade los accesos directos a su Men� de Inicio."
LangString strStartMenuDesc ${LANG_DANISH} "Dette tilf�jer genvejen i Start Menuen.."
LangString strStartMenuDesc ${LANG_DUTCH} "Dit voegt een snelkoppeling toe aan het startmenu."
LangString strStartMenuDesc ${LANG_CZECH} "P�id� z�stupce do menu Start."
LangString strStartMenuDesc ${LANG_RUSSIAN} "�������� ������ ������� � ���� ���������."
LangString strStartMenuDesc ${LANG_BULGARIAN} "������ ������ �� ����� ���������� � Start Menu."


LangString strDesktopDesc ${LANG_ENGLISH} "This adds a shortcut to your desktop."
LangString strDesktopDesc ${LANG_FINNISH} "T�m� lis�� pikakuvakkeen ty�p�yd�llesi."
LangString strDesktopDesc ${LANG_POLISH} "Ta opcja dodaje skr�ty na pulpit."
LangString strDesktopDesc ${LANG_GERMAN} "Eine Verkn�pfung wird am Desktop hinzugef�gt."
LangString strDesktopDesc ${LANG_FRENCH} "Ceci ajoute un raccourci sur votre bureau."
LangString strDesktopDesc ${LANG_TRADCHINESE} "�o�N�|�s�W���|��z���ୱ�W�C"
LangString strDesktopDesc ${LANG_SIMPCHINESE} "�����������ӿ�ݷ�ʽ"
LangString strDesktopDesc ${LANG_SLOVAK} "Prid� z�stupcu na plochu."
LangString strDesktopDesc ${LANG_SPANISH} "A�ade un acceso directo a su Escritorio."
LangString strDesktopDesc ${LANG_DANISH} "Dette tilf�jer genvejen p� Skrivebordet."
LangString strDesktopDesc ${LANG_DUTCH} "Dit voegt een snelkoppeling toe uw bureaublad."
LangString strDesktopDesc ${LANG_CZECH} "P�id� z�stupce na plochu."
LangString strDesktopDesc ${LANG_RUSSIAN} "�������� ������ �� ������� �����."
LangString strDesktopDesc ${LANG_BULGARIAN} "������ ������ �� ����� ���������� �� Desktop."


LangString strStartupDesc ${LANG_ENGLISH} "This adds a shortcut to your startup folder so that Rainlendar start with Windows."
LangString strStartupDesc ${LANG_FINNISH} "T�m� lis�� pikakuvakeen Aloitus-hakemistoon, jotta Rainlendar k�ynnistyy Windowsin kanssa."
LangString strStartupDesc ${LANG_POLISH} "Ta opcja dodaje skr�ty do folderu Autostart, �eby Rainlendar by� uruchamiany przy starcie Windows."
LangString strStartupDesc ${LANG_GERMAN} "Eine Verkn�pfung wird im Startmen� hinzugef�gt und Rainlendar wird automatisch mit Windows gestartet."
LangString strStartupDesc ${LANG_FRENCH} "Ceci ajoute un raccourci dans votre dossier de d�marrage, afin que Rainlendar d�marre avec Windows."
LangString strStartupDesc ${LANG_TRADCHINESE} "�o�N�|�s�W���|��z���Ұʥؿ����A�ҥH Rainlendar �N�|���H Windows �@�_�ҰʡC"
LangString strStartupDesc ${LANG_SIMPCHINESE} "��Windows��ʼʱһ������."
LangString strStartupDesc ${LANG_SLOVAK} "Spusti� pri �tartu - umiestn� z�stupcu do z�lo�ky po spusten�."
LangString strStartupDesc ${LANG_SPANISH} "A�ade un acceso directo a su carpeta de Inicio de forma que Rainlendar se inicie con Windows."
LangString strStartupDesc ${LANG_DANISH} "Dette tilf�jer genvejen til din Startmappe s� Rainlendar starter sammen med Windows."
LangString strStartupDesc ${LANG_DUTCH} "Dit voegt een snelkoppeling toe aan uw opstartmenu zodat deze direct start met Windows."
LangString strStartupDesc ${LANG_CZECH} "Spustit p�i startu - um�st� z�stupce do slo�ky po spu�t�n�."
LangString strStartupDesc ${LANG_RUSSIAN} "�������� ������ � ����� ������������ ��� ��������������� ������� Rainlendar ������ � Windows."
LangString strStartupDesc ${LANG_BULGARIAN} "������ ������ ��� ������ Startup ���������� ����, �� Rainlendar �� �� �������� ������ � Windows."


LangString strClose ${LANG_ENGLISH} "Please close Rainlendar before you proceed."
LangString strClose ${LANG_FINNISH} "Sulje Rainlendar ennen kuin jatkat."
LangString strClose ${LANG_POLISH} "Prosz� zamkn�� Rainlendar zanim b�dziesz kontynuowa�."
LangString strClose ${LANG_GERMAN} "Bitte Beenden Sie Rainlendar bevor Sie fortfahren."
LangString strClose ${LANG_FRENCH} "Merci de fermer Rainlendar avant de continuer."
LangString strClose ${LANG_TRADCHINESE} "�b���榹�{���e�Х����� Rainlendar �C"
LangString strClose ${LANG_SIMPCHINESE} "���������ȹر�Rainlendar."
LangString strClose ${LANG_SLOVAK} "Ukon�ite Rainlendar sk�r ne� budete pokra�ova�."
LangString strClose ${LANG_SPANISH} "Por favor, cierre Rainlendar antes de proceder."
LangString strClose ${LANG_DANISH} "V�r venlig at lukke Rainlendar f�r du forts�tter."
LangString strClose ${LANG_DUTCH} "Sluit Rainlendar af voor u verder gaat."
LangString strClose ${LANG_CZECH} "Ukon�ete Rainlendar ne� budete pokra�ovat."
LangString strClose ${LANG_RUSSIAN} "����������, �������� Rainlendar � ������� ������ ���������."
LangString strClose ${LANG_BULGARIAN} "���� ��������� Rainlendar ����� �� ����������."


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

