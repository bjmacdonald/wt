WT_ADD_EXAMPLE(treeviewdragdrop.wt
  CsvUtil.C
  FolderView.C
  TreeViewDragDrop.C
)
IF(TARGET Boost::headers)
  TARGET_LINK_LIBRARIES(treeviewdragdrop.wt Boost::headers)
ENDIF()

INCLUDE_DIRECTORIES(
  ${WT_SOURCE_DIR}/src
)
