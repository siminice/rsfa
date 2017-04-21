/* 
 * Copyright (c) 1994, 2014, Radu I. Siminiceanu. All rights reserved.
 */

package info.rsfa.model;

import java.util.Comparator;
import java.util.TreeMap;

import org.joda.time.DateTime;
import org.joda.time.Interval;

/**
 * @author Radu Siminiceanu
 */

public class AliasTimeline extends TreeMap<Interval, Alias> {
    private static final Comparator<Interval> compStartDate = new Comparator<Interval>() {
        @Override
        public int compare(Interval i1, Interval i2) {
            final long diff = i1.getStartMillis() - i2.getStartMillis();
            if (diff < 0L)
                return -1;
            else if (diff == 0L)
                return 0;
            else
                return 1;
        }
    };

    public AliasTimeline() {
        super(compStartDate);
    }

    public Alias getAlias(final DateTime t) {
        return null;
    }

    public String getName(final DateTime t) {
        return null;
    }

    public String getNick(final DateTime t) {
        return null;
    }

}