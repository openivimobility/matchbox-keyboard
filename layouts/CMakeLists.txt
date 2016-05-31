set(LAYOUTS
  keyboard-small.xml
  base-fragment.xml
  base-fragment-en_US.xml
  base-fragment-ar_EG.xml
  base-fragment-ca_ES.xml
  base-fragment-es_ES.xml
  base-fragment-ru_RU.xml
  base-fragment-cs_CZ.xml
  base-fragment-da_DK.xml
  base-fragment-de_DE.xml
  base-fragment-el_GR.xml
  base-fragment-et_EE.xml
  base-fragment-fi_FI.xml
  base-fragment-fr_FR.xml
  base-fragment-it_IT.xml
  base-fragment-lv_LV.xml
  base-fragment-nb_NO.xml
  base-fragment-nl_NL.xml
  base-fragment-pl_PL.xml
  base-fragment-pt_PT.xml
  base-fragment-sk_SK.xml
  base-fragment-sl_SI.xml
  base-fragment-sv_SE.xml
  base-fragment-tr_TR.xml
  internet-fragment.xml
  keyboard.xml
  keyboard-lq1.xml
  keyboard-symbol.xml
  keyboard-numpad-extended.xml
  keyboard-numpad-small.xml
  keyboard-internet.xml
)

set(datadir ${DATADIR})


foreach(LAYOUT ${LAYOUTS})
  configure_file(${LAYOUT}.in ${LAYOUT})
  install(
    FILES
    ${CMAKE_CURRENT_BINARY_DIR}/${LAYOUT}
    DESTINATION ${DATADIR}
  )
endforeach(LAYOUT)

set(IMGS
  arrow.png
  arrows.png
  return.png
  shift.png
  shift_2.png
  shift_3.png
  shift_caps.png
  123.png
  super_0.png
  sub_0.png
  super_1.png
  sub_1.png
  super_2.png
  sub_2.png
  super_3.png
  sub_3.png
  super_4.png
  sub_4.png
  super_5.png
  sub_5.png
  super_6.png
  sub_6.png
  super_7.png
  sub_7.png
  super_8.png
  sub_8.png
  super_9.png
  sub_9.png
  super_plus.png
  sub_plus.png
  super_minus.png
  sub_minus.png
  delta_cap.png
  degree.png
)


install(
  FILES
  ${CMAKE_CURRENT_BINARY_DIR}/keyboard-small.xml
  ${IMGS}
  DESTINATION ${DATADIR}
)
