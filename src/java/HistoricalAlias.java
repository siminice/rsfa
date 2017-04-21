/* 
 * Copyright (c) 1994, 2014, Radu I. Siminiceanu. All rights reserved.
 */

package info.rsfa.model;

import org.joda.time.DateTime;
import org.joda.time.Interval;

/**
 * @author Radu Siminiceanu
 */

public class HistoricalAlias {

    private final Alias alias;
    private final Interval interval;

    public HistoricalAlias(final Alias alias, final Interval interval) {
        this.alias = new Alias(alias);
        this.interval = interval;
    }

    public HistoricalAlias(final Alias alias, final DateTime fromDate,
            DateTime toDate) {
        this.alias = new Alias(alias);
        this.interval = new Interval(fromDate, toDate);
    }

    public HistoricalAlias(final String full, final String nick,
            final Interval interval) {
        this.alias = new Alias(full, nick);
        this.interval = interval;
    }

    public HistoricalAlias(final String full, final String nick,
            final DateTime fromDate, DateTime toDate) {
        this.alias = new Alias(full, nick);
        this.interval = new Interval(fromDate, toDate);
    }

    @Override
    public String toString() {
        StringBuilder sb = new StringBuilder();
        sb.append(alias.toString()).append(interval.toString());
        return sb.toString();
    }
}