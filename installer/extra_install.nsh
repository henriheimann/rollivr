# ----------------------------------- Add driver to openvrpaths.vrpath -----------------------------
Var /GLOBAL inst_dir_driver
Var /GLOBAL inst_dir_driver_escaped

StrCpy $inst_dir_driver "$INSTDIR\driver\rollivr"
${StrRep} $inst_dir_driver_escaped $inst_dir_driver "\" "\\"

nsJSON::Set /file "$PROFILE\\AppData\\Local\\openvr\\openvrpaths.vrpath" /end

# Special case if external_drivers is null
nsJSON::Get `external_drivers` /end
Pop $R0
${If} $R0 == null
  nsJSON::Set `external_drivers` /value `[ "$inst_dir_driver_escaped" ]` /end
  Goto done
${EndIf}

${For} $R1 0 100
  nsJSON::Get /exists `external_drivers` /index $R1 /end
  Pop $R0
  ${If} $R0 == no
    nsJSON::Set `external_drivers` /index $R1 /value `"$inst_dir_driver_escaped"` /end
    Goto done
  ${Else}
    nsJSON::Get `external_drivers` /index $R1 /end
    Pop $R0
    ${If} $R0 == $inst_dir_driver
      Goto done
    ${EndIf}
  ${EndIf}
${Next}

done:
nsJSON::Serialize /format /file "$PROFILE\\AppData\\Local\\openvr\\openvrpaths.vrpath"


# ----------------------------------- Install manifest ---------------------------------------------
# Installs manifest and sets the overlay to autostart
nsExec::ExecToLog '"$INSTDIR\install_manifest.bat"'

# If SteamVR is already running, manually perform the first start
FindWindow $0 "Qt5QWindowIcon" "SteamVR Status"
StrCmp $0 0 +2
    Exec '"$INSTDIR\overlay_rollivr.exe"'