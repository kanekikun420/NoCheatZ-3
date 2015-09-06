#ifndef NCZ_PREPROCESSORS
#define NCZ_PREPROCESSORS

#ifdef WIN32
#define PLAT "Windows"
#else
#define PLAT "Linux"
#endif

#ifdef NCZ_CSS
#define NCZ_VERSION_STR	"3.3.49.144 r58 CS:S"
#define PIPELINE_ADDRESS "pipeline.nocheatz.com"
#define GAMEDIR "cstrike"
#endif
#ifdef NCZ_CSP
#define NCZ_VERSION_STR	"3.3.0.0 b1 Beta CS:Promod"
#define PIPELINE_ADDRESS "pipeline-csp.nocheatz.com"
#define GAMEDIR "cspromod"
#endif
#ifdef NCZ_CSGO
#define NCZ_VERSION_STR	"3.3.0.0 b1 Beta CS:GO"
#define PIPELINE_ADDRESS "pipeline-csgo.nocheatz.com"
#define GAMEDIR "csgo"
#endif

/////////////////////////////////////////////////////
//// UPDATE THIS WHEN YOU UPDATE THE PLUGIN /////////
/////////////////////////////////////////////////////
//#define DEV " (Debug)"

#ifndef DEV
#define DEV ""
#else
#define SETDEV
#endif

#ifdef WIN32 // verify often ( http://www.sourcemodplugins.org/vtableoffsets )
//#define VTABLE_CMD 417
#define VTABLE_CMD 408
#define VTABLE_SHOULD_TRANSMIT 18
#define VTABLE_UPDATE_TRANSMIT 19
#else
//#define VTABLE_CMD 418
#define VTABLE_CMD 409
#define VTABLE_SHOULD_TRANSMIT 19
#define VTABLE_UPDATE_TRANSMIT 20
#endif

//DEV
//#define NCZ_VERSION_INT 301030080
/* TODO :
- Affichage des steam id lors de la connexion.
- Anti connect flood.
- Anti-NoFlash.
- Anti RadarHack.
- Détecteur SpeedHack.
- Séparation en class polymorphes des systèmes de détection.
*/

/* 3.3.49.142 r56 CS:S
- Suppression du flood lors d'une détection pour convar bypass
- Compatibilité avec les plugins SlowMotion
- Ajout d'une commande pour éviter l'auto restart en cas de mise à jour (ncz_update_norestart), utile si votre serveur redémarre déjà périodiquement.
- Mise à jour de nocheatz_autoexec.cfg

/* 3.3.48.140 r55 CS:S
- Modification du détecteur de triggerbot
- Modification du détecteur de bunnyhop script
- Automatisation des bannissements pour aimbot / convar bypass.
- Mise à jour du détecteur de convar bypass
- Mise à jour du bloqueur d'aimbot
- Build x64 pour linux
*/

/* 3.3.47.138 r54 CS:S
- Ajout de variables consoles interdites.
- Correction du lien affiché dans le message de kick.
- Suppression du vérificateur de rates, devenu inutile.
- Suppression de la compatibilité avec Mani Admin Plugin.
- (En Test) Changement de la façon d'entretenir les bans avec SourceMod/SourceBans.
- Correction d'un crash lors du refus du chargement du plugin.
- Correction du redémarrage forcé du serveur.
*/

/* 3.3.46.132 r53 CS:S
- Suppression des systèmes anti-aimbot 2 & 3.
- Suppression du guardien sv_consistency.
- Désactivation temporaire du bloqueur d'aimbot.
- Mise à jour du vérificateur de convars.
- Modification de nocheatz_autoexec.cfg.
- Ajout d'un lien vers la page de rapports NoCheatZ dans le message de bannissement, si ncz_report_cheat est actif.

*/
/* 3.3.45.126 r52 CS:S
+ Evite le flood dans les logs.
+ Rétablissement du bloqueur d'aimbot sous Linux.
+ Mise à jour de nocheatz_autoexec.cfg
+ Ne coupe plus le serveur lorsque des joueurs sont présents et qu'une mise à jour est disponible.

3.3.45.114 r46
- Blocage de mp_playerid à 2.
- Correction du système de vérification des Convars qui faisait lag le jeu.
- Ajout de l'option +ncz_changelevel_at_start à la commande de lancement pour activer ou non le changement de map automatique au lancement du serveur. (1 - Activer, 2 - Ne pas autoriser)
- Correction des connexions sous Windows.
- Optimisations
*/
/* 3.3.43.111 r45
- Modification des commandes serveur ncz_check_aimbots et ncz_banlists, voir cfg/nocheatz/nocheatz_autoexec.cfg
- Optimisations
- Suppression anti-aimbot par vitesse de visée.
*/
/* 3.3.42.109 r44
- Correction d'un crash
*/
/* 3.3.42.108 r43
- Calibrage
*/
/* 3.3.42.107 r42
- Ajout du rechargement automatique de la map au lancement du serveur, puis après l'activation SourceTV si nécessaire.
- Ajout du système anti-aimbot (Vitesse de visée)
- Ajout du système bloqueur d'aimbot.
- Amélioration de l'auto-update.
- Correction anti bunny hop script.
- Correction d'un second crash au changement de map.
- Correction d'un problème de connexion sous Windows.
*/
/* 3.2.39.103 r41
- Compatibilité avec libc concernant gethostbyname(). Nécessite la réinstallation manuelle du plugin et de l'autoupdate.
- Ajout des banlists ESL, ESEA, EAC.

*/
/*  3.2.37.101 r39
- Amélioration de l'anti bunny hop script
*/
/*  3.2.36.99 r38
- Petite correction de l'auto status
- Mise à jour des liens vers le serveur maître (Réinstallation de l'auto-update)
*/
/*  3.2.36.99 r37
- Correction de la vérification de l'angle Z (Roll) contre les fausses détections dues aux plugins d'administration, lors du Drug.
*/
/*  3.2.35.98 r36
- Amélioration du nouveau anti-aimbot.
- Ajout de l'angle Z (Roll) à vérifier dans l'ancien anti aimbot.
- Correction de l'anti TriggerBot.
*/
/*  3.2.34.95 r35
- Mise à jour de l'ancien anti aimbot pour déteter les anomalies dans l'angle Z.
*/
/*  3.2.34.95 r35
- Mise à jour du nouveau détecteur contre les fausses détections
*/
/*  3.2.34.94 r34
- 2 joueurs minimum avant de lancer la record auto.
- Nouveau détecteur d'aimbot.
- Changement de l'extension des fichiers logs en .log
- Correction du temps de connexion des joueurs (Mauvaise initialisation d'une variable)
*/
/*  3.1.33.91 r33
- Correction d'un crash avec l'auto status.
- Correction des connexions au serveur.
*/
/*  3.1.33.89 r32
- Correction du redémarrage automatique pour les mises à jour.
*/
/*  3.1.33.88 r31
- Vérifie si une mise à jour est disponbile en fin de round puis reboot le serveur automatiquement sous Linux si une mise à jour est disponible et si personne n'est sur le serveur.
- Fix pour éviter de clôner les threads.
- Correction de la compatibilité avec SourceBans.
- Correction d'un crash
/*  3.1.32.85 r30
- Correction d'un crash lors de la déconnexion d'un joueur
- Ajout Anti Name Changer
*/
/*  3.1.31.84 r29
- Limite le travail du vérificateur de convars
- Anti-flood sur l'auto status et commande client ncz_status.
*/
/*  3.1.31.82 r28
- Ajout de la commande ncz_allow_status (Autorise ou non autostatus et commande client ncz_status)
- Correction d'un flood dans les logs lors du bannissement.
- Correction des ajouts de bannissements pour les plugins d'administration.
- Correction d'un bug dans le compteur de détections pour TriggerBot.
*/

/* 3.1.26.73 r27
- Ajout de l'Auto Update (Fonctionne au démarrage du serveur)
- Allégement du vérificateur de commandes console inconnues.
- Ajout de convars à vérifier.
- Les convars à vérifier ne sont plus modifiables.
- Personne ne pourra se connecter si un SSH est détecté sur le serveur.
- Supression des messages GameFrame
- Suppression des messages Probably dans les logs.
- Désactivation de l'anti SpeedHack
- Optimisations
- Fix d'un détecteur qui s'éxécutait 2 fois.
- Fix des erreurs en fin de round qui kickait tout le monde.
- Fix d'un thread qui utilisait toute la charge CPU.

- Fin de la Bêta.
*/
/* 3.1.25.69 r25
- Kick les utilisateurs de LSS et hldj.
- La téléportation ne génére plus de faux positif.
*/
/* 3.1.25.69 r24
- Ajout de la commande wait au niveau serveur.
- Kick les utilisateurs de openscript
- Rétablissement de l'enregistrement automatique Source TV.
- Rétablissement du système anti-aimbot.
- Correction d'un bug lorsqu'un joueur banni est kické.
- Correction des déconnexions mal prises en compte.
*/
/* 3.1.23.67 r23
- Ajout de commandes clientes connues.
- Ajout d'un système de post-détection des crash.
- Vérification du code, correction des Warning.
- Affiche l'adresse IP du joueur dans les logs NoCheatZ 3 lorsqu'il se connecte.

- Allégement des logs.
- Fix d'un bug avec le comptage du nombre de détections.
- N'envoie plus d'alerte de commande console inconnue pour les cvar.
- Fix d'un crash lors de connexion.
- Suppression des calculs inutiles pour l'anti aimbot.
*/
/* 3.1.19.61 r22
- Correction d'un bug avec les commandes clientes inconnues.
*/
/* 3.1.19.60 r21
- Correction de l'affichage de message de détections lors d'une déconnexion d'un joueur.
- Ajout d'une marge pour le système de dépassement du temps d'éxecution.
- Suppression de la mémoire inutile.
- Modificiation du code pour utiliser des float strictes.
- La vérification de la version se fait dans un thread.
- Autres corrections
*/
/* 3.1.19.54 r20
- Correction d'un crash avec la nouvelle vérification pour le système anti TriggerBot.
*/
/*
- Correction des systèmes de message dans le chat (Textes mal placés / Overflow va_list).
- Correction d'un doublon dans les logs lors de détections.
- Modification du comportement lors de la vérification d'un joueur dans une banlist.
- Ajout d'un test de dépassement de vitesse (spatial) pour le système anti speedhack.
- Ajout d'un test qui vérifie si un adversaire est dans le viseur lors d'une détection triggerbot, sait différencier l'Autopistol et le Triggerbot.
- Correction de la protection contre les fausses détections Aimbot.
*/

 // 3 . 00 . 000 . 000
#define NCZ_PLUGIN_NAME "NoCheatZ 3"

#define NCZ_REPORT_FIRST_LINE NCZ_PLUGIN_NAME " (" NCZ_VERSION_STR ") - HOSTNAME : " + tostring(g_pCVar->FindVar("hostname")->GetString()) + " - PORT : " + tostring(g_pCVar->FindVar("hostport")->GetString()) + " - SYSTEM : " + PLAT + " - GAME : " + tostring(gamedll->GetGameDescription()) + " - MAP : " + getMapName() +  " - Date Time : " + getStrDateTime("%x %X") + "\n";

#define compa(a) if(V_strcmp(cmd, a) == 0)\
{\
return false;\
}

#define scomp(a) if(commandline.find(a) != std::string::npos)\
{\
return true;\
}

#define getClCV(a, b) engine->GetClientConVarValue(a, b)

#include <assert.h>

#define STOP_HERE ASSERT(0)

#define Y_CHECK(a, b) ((!(a > 0.0f && b < 0.0f) && !(a < 0.0f && b > 0.0f)) && (!(a > -180.0f && b < 0.0f) && !(a < 0.0f && b > 0.0f)))

#define IN_ATTACK		(1 << 0)
#define IN_JUMP (1<<1) 

#ifdef NCZ_EP1
#define RETRIEVE_ARGV(a) engine->Cmd_Argv(a)
#define RETRIEVE_ARGS engine->Cmd_Args()
#define RETRIEVE_ARGC engine->Cmd_Argc()
#define FINDCOMMAND(a) g_pCVar->GetCommands()->FindCommand(a)
#define IsFlagSet IsBitSet
#else
#define RETRIEVE_ARGV(a) args.Arg(a)
#define RETRIEVE_ARGC args.ArgC()
#define RETRIEVE_ARGS args.GetCommandString()
#define FINDCOMMAND(a) g_pCVar->FindCommand(a)
#endif

#endif

