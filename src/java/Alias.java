/* 
 * Copyright (c) 1994, 2014, Radu I. Siminiceanu. All rights reserved.
 */

package info.rsfa.model;

/**
 * @author Radu Siminiceanu
 */

public class Alias {

    private static final String NICK_MARKER = "~";
    private static final String NICK_START_SEP = "(";
    private static final String NICK_END_SEP = ")";

    private final String fullName;
    private final String nickName;

    public Alias(final String full, final String nick) {
        this.fullName = new String(full);
        this.nickName = new String(nick);
    }

    public Alias(final Alias alias) {
        this.fullName = new String(alias.getFullName());
        this.nickName = new String(alias.getNickName());
    }

    public String getFullName() {
        return fullName;
    }

    public String getNickName() {
        return nickName;
    }

    @Override
    public String toString() {
        StringBuilder sb = new StringBuilder();
        sb.append(fullName).append(NICK_MARKER).append(NICK_START_SEP)
                .append(nickName).append(NICK_END_SEP);
        return sb.toString();
    }
}