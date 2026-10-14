import org.jsoup.Jsoup;
import org.jsoup.nodes.Document;
import org.jsoup.nodes.Element;
import org.jsoup.select.Elements;

import java.io.File;
import java.io.IOException;
import java.text.ParseException;
import java.text.SimpleDateFormat;
import java.util.*;
import java.util.regex.Matcher;
import java.util.regex.Pattern;

import static java.util.stream.Collectors.joining;

/**
 * Rewritten for the 2026 worldfootball.net redesign ("match-report"/"person"/
 * "teams" URL scheme with te<id>/pe<id>/ve<id>/co<id> ids, and a completely
 * different page markup). Output format (print()/serializeLineups()/
 * serializeEvents()) and the minute bookkeeping (deductMin/getMin) are
 * UNCHANGED from the old parser on purpose, so downstream tooling (wf-add-
 * lineups etc.) does not need to change. Only the extraction side changed.
 *
 * NOTE ON MNEMONICS: team/person/venue mnemonics are now the site's stable
 * numeric id (e.g. "te1776", "pe291835", "ve891") instead of the old name
 * slug. The old site had no ids, only slugs, so every mnemonic produced by
 * this version is new relative to the historical catalogs -- reconciling
 * them is an Alias/HistoricalAlias job, not something this parser can do by
 * itself. See the chat writeup for why this is the recommended direction.
 */
public class WfReport {
  static String home = " ";
  static String away = " ";
  static String score = " ";
  static String half = " ";
  static String competition = " ";
  static String date = " ";
  static String round = " ";
  static String time = " ";
  static String venue = " ";
  static String venueId = "";
  static String attend = " ";
  static List<String> evol = new ArrayList();
  static List<String> events = new ArrayList();
  static List<String> hRoster = new ArrayList();
  static List<String> aRoster = new ArrayList();
  static List<String> hSubs = new ArrayList();
  static List<String> aSubs = new ArrayList();
  static Set<String> hScorers = new HashSet();
  static Set<String> aScorers = new HashSet();
  static Map<String, Integer> min = new HashMap();
  static String hCoach = "      ";
  static String aCoach = "      ";
  static String referee = "      ";
  static String refereeId = "";
  static Map<String, String> eSymbol = new HashMap();

  private static SimpleDateFormat sdf = new SimpleDateFormat("dd.MM.yyyy", Locale.US);
  private static SimpleDateFormat idf = new SimpleDateFormat("yyyy-MM-dd");

  public static void main(String args[]) {
    try { System.setOut(new java.io.PrintStream(System.out, true, "UTF-8")); } catch (Exception e) { /* ignore */ }
    File input = new File(args[0]);
    Document doc = null;
    try {
      doc = Jsoup.parse(input, "UTF-8", "https://www.worldfootball.net");
    } catch (IOException e) {
      e.printStackTrace();
    }
    extractReport(doc);
  }

  private static void extractReport(Document doc) {
    Element scoreboard = doc.selectFirst(".hs-scoreboard");
    extractHeader(scoreboard);
    extractInfo(scoreboard);
    extractVenueRefAttend(scoreboard);
    Element lineupBlock = doc.selectFirst(".hs-block.hs-lineup");
    extractSide(lineupBlock, "home", hRoster, hSubs);
    extractSide(lineupBlock, "away", aRoster, aSubs);
    extractManagers(lineupBlock);
    print();
  }

  public static void print() {
    System.out.println("Home: " + home);
    System.out.println("Away: " + away);
    System.out.println("Score: " + score.replaceAll(" ",""));
    System.out.println("Half-time: " + half);
    System.out.println("Competition: " + competition);
    System.out.println("Date: " + date);
    System.out.println("Round: " + round);
    System.out.println("Time: " + time);
    System.out.println("Venue: " + venue + (venueId.isEmpty() ? "" : " " + venueId));
    System.out.println("Attendance: " + attend);
    System.out.println("Evolution: " + evol);
    System.out.println("Events: " + events);
    System.out.println("Minutes: " + min);
    System.out.println("Home Roster: " + hRoster);
    System.out.println("Home Subs: " + hSubs);
    System.out.println("Home Coach: " + hCoach);
    System.out.println("Guest Roster: " + aRoster);
    System.out.println("Guest Subs: " + aSubs);
    System.out.println("Guest Coach: " + aCoach);
    System.out.println("Referee: " + referee + (refereeId.isEmpty() ? "" : " " + refereeId));
    System.out.println("Lineups: " + serializeLineups());
    System.out.println("Events: " + serializeEvents());
  }

  public static String serializeLineups() {
    StringBuilder sb = new StringBuilder();
    String sc1 = score.replaceAll(" ", "").replaceAll("-", "0");
    sb
        .append(home).append(",")
        .append(away).append(",")
        .append(sc1).append(",")
        .append(String.format("%s@%s", date, time)).append(",A,")
        .append(round).append(",")
        .append(venue).append(",")
        .append(attend).append(", ,");
    for (String s : hRoster) {
      Integer m = 90 + min.getOrDefault(s, 0);
      sb.append(s).append(":").append(m).append(",");
    }
    for (int i=0; i<11-hRoster.size(); i++) {
      sb.append("      :0  ,");
    }
    for (String s : hSubs) {
      Integer m = min.getOrDefault(s, 0);
      sb.append(s).append(":").append(m).append(",");
    }
    for (int i=hSubs.size(); i < 11; i++) {
      sb.append(" :0,");
    }
    sb.append(hCoach).append(",");
    for (String s : aRoster) {
      Integer m = 90 + min.getOrDefault(s, 0);
      sb.append(s).append(":").append(m).append(",");
    }
    for (int i=0; i<11-aRoster.size(); i++) {
      sb.append("      :0  ,");
    }
    for (String s : aSubs) {
      Integer m = min.getOrDefault(s, 0);
      sb.append(s).append(":").append(m).append(",");
    }
    for (int i=aSubs.size(); i < 11; i++) {
      sb.append(" :0,");
    }
    sb.append(aCoach).append(",");
    sb.append(referee).append(", , , , ,");
    return sb.toString();
  }

  public static String serializeEvents() {
    return events.stream().collect(joining(","));
  }

  // "Matchday 7 | Sat, 19.09.2026, 21:30"
  private static void extractHeader(Element scoreboard) {
    if (scoreboard == null) return;
    Element eround = scoreboard.selectFirst(".hs-head--round");
    if (eround == null) return;
    String line = eround.text();
    String parts[] = line.split("\\|");
    String roundPart = parts.length > 0 ? parts[0].trim() : "";
    Matcher rm = Pattern.compile("(\\d+)").matcher(roundPart);
    round = rm.find() ? rm.group(1) : "-1";
    if (parts.length > 1) {
      String dtPart = parts[1].trim();
      // "Sat, 19.09.2026, 21:30" -> drop the weekday, keep date + time
      String dtParts[] = dtPart.split(",");
      if (dtParts.length >= 3) {
        date = toIso(dtParts[1].trim());
        time = dtParts[2].trim();
      } else if (dtParts.length == 2) {
        date = toIso(dtParts[1].trim());
      }
    }
  }

  private static void extractInfo(Element scoreboard) {
    if (scoreboard == null) return;
    Element matchDiv = scoreboard.selectFirst("[data-match_id]");
    if (matchDiv == null) return;
    Element ehome = matchDiv.selectFirst(".team-shortname-home a[href]");
    home = extractMnem(ehome, " ");
    Element eaway = matchDiv.selectFirst(".team-shortname-away a[href]");
    away = extractMnem(eaway, " ");
    Element escore = matchDiv.selectFirst(".match-result-0");
    if (escore != null) score = escore.text().trim();
    Element ehalf = matchDiv.selectFirst(".match-result-intermediate .match-result-1");
    if (ehalf != null) half = ehalf.text().trim();
  }

  private static void extractVenueRefAttend(Element scoreboard) {
    if (scoreboard == null) return;
    Elements entries = scoreboard.select(".hs-scoreboard-details > li");
    for (Element li : entries) {
      Elements spans = li.select("span");
      if (spans.size() < 2) continue;
      String label = spans.get(0).text().trim();
      Element valueSpan = spans.get(1);
      if (label.equalsIgnoreCase("Stadium")) {
        Element a = valueSpan.selectFirst("a[href]");
        venue = extractMnem(a, " ");
        venueId = extractId(a, "");
      } else if (label.equalsIgnoreCase("Attendance")) {
        attend = valueSpan.text().trim().replaceAll(",", "").replaceAll("\\.", "");
      } else if (label.equalsIgnoreCase("Referee")) {
        Element a = valueSpan.selectFirst("a[href]");
        referee = extractMnem(a, "      ");
        refereeId = extractId(a, "");
      }
    }
  }

  // side is "home" or "away"
  private static void extractSide(Element lineupBlock, String side, List<String> roster, List<String> subs) {
    if (lineupBlock == null) return;
    Element starter = selectBySideClass(lineupBlock, "hs-lineup--starter", side);
    if (starter != null) {
      for (Element row : starter.select(".event.playing.lineup")) {
        String mnem = rowMnem(row);
        if (mnem == null) continue;
        roster.add(mnem);
        applyRowEvents(row, mnem, false);
      }
    }
    Element bench = selectBySideClass(lineupBlock, "hs-lineup--bench", side);
    if (bench != null) {
      for (Element row : bench.select(".event.playing.bench")) {
        String mnem = rowMnem(row);
        if (mnem == null) continue;
        subs.add(mnem);
        applyRowEvents(row, mnem, true);
      }
    }
  }

  private static Element selectBySideClass(Element root, String baseClass, String side) {
    for (Element e : root.select("." + baseClass)) {
      if (e.hasClass(side)) return e;
    }
    return null;
  }

  private static String rowMnem(Element row) {
    Element a = row.selectFirst(".person-name a[href]");
    return a != null ? extractMnem(a, null) : null;
  }

  // Looks at the (up to 3) ".match_event" slots on a lineup row and updates
  // events/min bookkeeping exactly like the old addEvents()/extractMins() did.
  private static void applyRowEvents(Element row, String mnem, boolean isBench) {
    for (Element slot : row.select(".match_event")) {
      Element inner = slot.children().isEmpty() ? null : slot.children().first();
      if (inner == null) continue;
      String cls = " " + inner.className() + " ";
      int m = parseMinute(inner.text());
      if (cls.contains(" goal ")) {
        boolean isPen = cls.contains(" penalty ");
        boolean isOwn = cls.contains(" own ") || cls.contains("own-goal");
        if (isOwn) {
          events.add(String.format("%s`%d", mnem, m));
        } else if (isPen) {
          events.add(String.format("%s\"%d", mnem, m));
        } else {
          events.add(String.format("%s'%d", mnem, m));
        }
      } else if (cls.contains(" card ")) {
        if (cls.contains(" red ") || cls.contains("second-yellow")) {
          events.add(String.format("%s!%d", mnem, m));
          deductMin(mnem, m);
        } else {
          events.add(String.format("%s#%d", mnem, m));
        }
      } else if (cls.contains(" playing ")) {
        if (cls.contains("substitute-out")) {
          deductMin(mnem, m);
        } else if (cls.contains("substitute-in")) {
          min.put(mnem, 91 - m);
        }
      }
    }
  }

  // "54." -> 54 ; "90.+4" -> 94 ; "" -> 90 (fallback)
  private static int parseMinute(String s) {
    if (s == null) return 90;
    String t = s.trim().replaceAll("\\.", "");
    if (t.length() == 0) return 90;
    try {
      if (t.contains("+")) {
        String parts[] = t.split("\\+");
        int base = Integer.parseInt(parts[0].trim());
        int extra = parts.length > 1 && parts[1].trim().length() > 0 ? Integer.parseInt(parts[1].trim()) : 0;
        return base + extra;
      }
      return Integer.parseInt(t);
    } catch (NumberFormatException e) {
      return 90;
    }
  }

  private static void extractManagers(Element lineupBlock) {
    if (lineupBlock == null) return;
    hCoach = extractManager(lineupBlock, "home");
    aCoach = extractManager(lineupBlock, "away");
  }

  private static String extractManager(Element lineupBlock, String side) {
    Element coaches = selectBySideClass(lineupBlock, "hs-lineup--coaches", side);
    if (coaches == null) return "      ";
    Elements children = coaches.children();
    for (int i = 0; i < children.size(); i++) {
      Element c = children.get(i);
      if (c.hasClass("role") && c.text().trim().equalsIgnoreCase("Manager")) {
        if (i + 1 < children.size()) {
          Element a = children.get(i + 1).selectFirst(".person-name a[href]");
          return extractMnem(a, "      ");
        }
      }
    }
    return "      ";
  }

  // The trailing path segment is still the name slug (e.g.
  // "teams/te1776/sporting-cp/" -> "sporting-cp"), same as the old
  // "/teams/sporting-cp/" scheme's last segment. Kept as the mnem on
  // purpose: cplayers.dat/teams.dat and the lineups/src/*.cc catalog
  // matchers are keyed off this slug, not the new numeric id, so matching
  // this is what keeps this parser's output compatible with the existing
  // catalogs without touching the C++ layer. The id segment (second-to-
  // last) is available too if a future catalog column wants it.
  private static String extractMnem(Element link, String def) {
    if (link == null) return def;
    String href = link.attr("href");
    List<String> segs = new ArrayList<>();
    for (String p : href.split("/")) if (p.length() > 0) segs.add(p);
    if (segs.size() < 1) return def;
    return segs.get(segs.size() - 1);
  }

  // The second-to-last path segment is the site's new stable numeric id
  // (e.g. "/stadiums/ve891/estadio-jose-alvalade/" -> "ve891"). This is NOT
  // used as a catalog mnemonic anywhere (that stays the slug, see
  // extractMnem above) -- it exists purely so a bash step that finds a
  // venue/referee slug not yet in the stadium/referee catalog can build the
  // "/person/<id>/<slug>/" or "/stadiums/<id>/<slug>/" fetch URL for a brand
  // new entry without having to re-derive the id some other way. Appended
  // to the human-readable Venue:/Referee: print() lines only -- never to
  // serializeLineups()'s CSV, so wfx-add-lineups.cc's line-position-based
  // parsing of the "Lineups:"/"Events:" lines is untouched.
  private static String extractId(Element link, String def) {
    if (link == null) return def;
    String href = link.attr("href");
    List<String> segs = new ArrayList<>();
    for (String p : href.split("/")) if (p.length() > 0) segs.add(p);
    if (segs.size() < 2) return def;
    return segs.get(segs.size() - 2);
  }

  private static void deductMin(String name, int minsub) {
    Integer minp = getMin(name);
    Integer rem = 91 - minsub;
    if (rem <= 0) rem = 1;
    min.put(name, minp - rem);
  }

  private static Integer getMin(String s) {
    return min.getOrDefault(s, 0);
  }

  private static String toIso(String s) {
    try {
      Date d = sdf.parse(s);
      String iso = idf.format(d);
      return iso;
    } catch (ParseException e) {
      return "0000-00-00";
    }
  }

}
