USES := libglog tidy

LIBS := tidyxx
tidyxx_STEMS := tidy

TESTS := tidy-test

# safty_flags := # nada
# visibility_flags := # nada
# lto_flags := # nada

include MKUltra/rules
