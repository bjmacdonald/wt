WT_ADD_EXAMPLE(client-ssl-auth.wt 
        SslClientAuth.C
	model/Session.C
	model/User.C
)
TARGET_LINK_LIBRARIES(client-ssl-auth.wt ${EXAMPLES_DBO_LIBS})
