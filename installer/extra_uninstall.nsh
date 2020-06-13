Var /GLOBAL uninst_dir_driver

StrCpy $uninst_dir_driver "$INSTDIR\driver\rollivr"

nsJSON::Set /file "$PROFILE\\AppData\\Local\\openvr\\openvrpaths.vrpath" /end

${For} $R1 0 100
  nsJSON::Get /exists `external_drivers` /index $R1 /end
  Pop $R0
  ${If} $R0 == no
    Goto done
  ${Else}
    nsJSON::Get `external_drivers` /index $R1 /end
    Pop $R0
    ${If} $R0 == $uninst_dir_driver
      nsJSON::Delete `external_drivers` /index $R1 /end
      IntOp $R1 $R1 - 1
    ${EndIf}
  ${EndIf}
${Next}

done:
nsJSON::Serialize /format /file "$PROFILE\\AppData\\Local\\openvr\\openvrpaths.vrpath"