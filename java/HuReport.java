import org.jsoup.Jsoup;
import org.jsoup.nodes.Document;
import org.jsoup.nodes.Element;
import org.jsoup.nodes.Node;
import org.jsoup.select.Elements;

import java.io.File;
import java.io.IOException;
import java.time.Month;
import java.util.*;

import static java.lang.Integer.min;
import static java.util.stream.Collectors.joining;

public class HuReport {
  static String home = " ";
  static String away = " ";
  static String score = " ";
  static String half = " ";
  static String competition = " ";
  static String date = " ";
  static String round = " ";
  static String time = " ";
  static String venue = " ";
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
  static Map<String, String> eSymbol = new HashMap();

  static {
    eSymbol.put("G", "'");
    eSymbol.put("PG", "\"");
    eSymbol.put("OG", "`");
    eSymbol.put("YC", "#");
    eSymbol.put("Y2C", "!");
    eSymbol.put("RC", "!");
    eSymbol.put("PM", "/");
  };

  public static void main(String args[]) {
    File input = new File(args[0]);
    Document doc = null;
    try {
      doc = Jsoup.parse(input, "UTF-8", "http://us.soccerway.com/");
    } catch (IOException e) {
      e.printStackTrace();
    }
    Element content = doc.selectFirst("#content");
    extractReport(content);
  }

  private static void extractReport(Element content) {
    Element header = content.selectFirst("h2");
    Element data = content.selectFirst("#match_data");
    Element roster = data.selectFirst("#spielinfos_box");
    Element goals = roster.selectFirst("#spielinfos");
    Element lineups = roster.selectFirst("#md_players_coaches");
    extractHeader(header);
    extractInfo(data);
    if (goals != null) extractEvents(goals);
    if (lineups != null) {
      hRoster.clear();
      aRoster.clear();
    } else {
      hScorers.forEach(s -> hRoster.add(s));
      aScorers.forEach(s -> aRoster.add(s));
    }
    extractLineups(lineups);
    extractSubs(lineups);
    print();
  }

  public static void print() {
    System.out.println("Home: " + home);
    System.out.println("Away: " + away);
    System.out.println("Score: " + score.replaceAll(" ",""));
    System.out.println("Half-time: " + half);
    System.out.println("Competition: " + competition);
    System.out.println("Date: " + toIso(date));
    System.out.println("Round: " + round);
    System.out.println("Time: " + time);
    System.out.println("Venue: " + venue);
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
    System.out.println("Referee: " + referee);
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
        .append(String.format("%s@%s", toIso(date), time)).append(",A,")
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

  private static void extractHeader(Element header) {
    Element h2 = header.selectFirst("h2");
    Element edate = h2.selectFirst("time");
    Element etime = h2.selectFirst(".kickoff");
    String sparts[] = h2.childNode(0).toString().split(" ");
    int nsparts = sparts.length;
    score = sparts[nsparts-3] + "-" + sparts[nsparts-1];
    date = extractText(edate, 0);
    time = extractText(etime, 0);
 }

  private static void extractInfo(Element data) {
    Element ecomp = data.selectFirst(".backlink-league");
    competition = extractText(ecomp, 0).replaceAll("\n", "");
    Element eround = data.selectFirst("#md_league");
    String roundinfo = extractText(eround, 0);
    String rparts[] = roundinfo.split(" ");
    if (rparts.length > 2) round = rparts[rparts.length-2].replaceAll("\\.", "");
    Element evenue = data.selectFirst("#md_ground");
    venue = extractLink(evenue, " ");
    Element eattend = data.selectFirst(".attendance");
    attend = extractText(eattend, 0).replaceAll("\\.", "");
    Element eref = data.selectFirst(".officials");
    referee = extractLink(eref, " ");
  }

  private static String extractText(Element e, int j) {
    if (e!=null && e.childNodeSize()>0) return e.childNode(j).toString();
    return " ";
  }

  private static String extractLink(Element e, String def) {
    if (e == null) return " ";
    Node n = e.selectFirst("a[href]");
    return extractLink(n, def);
  }

  private static String extractLink(Node n, String def) {
    if (n==null) return def;
    String link = n.attr("href");
    String[] parts = link.split("/");
    int np = parts.length;
    return parts[np-2] + "/" + parts[np-1];
  }

  private static String extractDisplayName(Element e) {
    if (e == null) return "";
    Element elink = e.selectFirst("a[href]");
    if (elink==null) return "";
    return elink.childNode(0).toString();
  }

  private static void extractEvents(Element e) {
    Elements elist = e.select("tr");
    hRoster.add("      ");
    aRoster.add("      ");
    for (Element row : elist) {
      Elements cols = row.select("td");
      if (cols.size() < 3) continue;
      Element epa = cols.get(0);
      Element eev = cols.get(1);
      Element epb = cols.get(2);
      String ev = extractEventType(eev);
      Element emina = epa.selectFirst(".min");
      String mina = extractText(emina, 0);
      String pan = extractLink(epa, "");
      String padn = extractDisplayName(epa);
      String moda = extractModifier(epa, pan, ev);
      Element eminb = epb.selectFirst(".min");
      String minb = extractText(eminb, 0);
      String pbn = extractLink(epb, "");
      String pbdn = extractDisplayName(epb);
      String modb = extractModifier(epb, pbn, ev);
      mina = mina.replaceAll(" ", "").replaceAll("\'", "");
      minb = minb.replaceAll(" ", "").replaceAll("\'", "");
      if (ev.length() > 1) {
        evol.add(mina + minb + ": " + padn + pbdn + " " + ev);
        if (pan.isEmpty()) {
          if (modb.length() > 1) {
            if (!modb.equals("`")) aScorers.add(pbn);
            else hScorers.add(pbn);
          }
        }
        if (pbn.isEmpty()) {
          if (moda.length() > 1) {
            if (!moda.equals("`")) hScorers.add(pan);
            else aScorers.add(pan);
          }
        }
      }
      if (ev.equals("!")) {
        String name = pan + pbn;
        Integer mins = getMin(name);
        Integer mine = extractMin(mina + minb);
        Integer rem = 91 - mine;
        if (rem <= 0) rem = 1;
        min.put(name, mins - rem);
      }
      Integer imin = extractMin(mina + minb);
      events.add(pan + pbn + moda + modb + imin);
    }
  }

  private static String extractEventType(Element e) {
    if (e == null || e.childNodeSize() == 0) return "";
    Node c = e.childNode(0);
    Elements ah = e.select("img");
    if (ah.isEmpty()) return c.toString();
    String codes = ah.stream().map(h->h.attr("src")).collect(joining("+"));
    if (codes.contains("red")) {
      return "!";
    }
    if (codes.contains("yellow")) return "#";
    return "";
  }

  private static void extractLineups(Element e) {
    Element tab = e.selectFirst("tbody");
    Elements teams = tab.select("th");
    home = extractLink(teams.get(0), "");
    away = extractLink(teams.get(1), "");
    Elements players = tab.selectFirst(".players").select("td");
    Elements coaches = tab.selectFirst(".coaches").select("td");
    hCoach = coaches.size() > 0 ? extractLink(coaches.get(0), " ") : " ";
    aCoach = coaches.size() > 1 ? extractLink(coaches.get(1), " ") : " ";

    if (players.size() == 0) return;
    Element hol = players.get(0).selectFirst("ol");
    Elements hrows = hol != null ? hol.select("li") : new Elements();
    for (int i = 0; i < min(11, hrows.size()); i++) {
      Element row = hrows.get(i);
      hRoster.add(extractPlayerStats(row));
    }

    if (players.size() < 1) return;
    Element aol = players.get(1).selectFirst("ol");
    Elements arows = aol != null ? aol.select("li") : new Elements();
    for (int i = 0; i < min(11, arows.size()); i++) {
      Element row = arows.get(i);
      aRoster.add(extractPlayerStats(row));
    }
  }

  private static void extractSubs(Element e) {
    Element tab = e.selectFirst("tbody");
    if (tab == null) return;
    Element players = tab.selectFirst(".players");
    if (players == null) return;
    Elements rows = players.select("td");
    Elements hsec = rows.size() > 0 ? rows.get(0).select("ol") : new Elements();
    Elements asec = rows.size() > 1 ? rows.get(1).select("ol") : new Elements();

    Elements hrows = hsec.size() > 1 ? hsec.get(1).select("li") : new Elements();
    for (Element row : hrows) {
      hSubs.add(extractPlayerStats(row));
    }
    Elements arows = asec.size() > 1 ? asec.get(1).select("li") : new Elements();
    for (Element row : arows) {
      aSubs.add(extractPlayerStats(row));
    }
  }
  private static String extractPlayerStats(Element e) {
    String name = extractLink(e, " ");
    String subinfo = extractSubInfo(e, name);
    return name;
  }

  private static String extractSubInfo(Element pl, String name) {
    if (pl == null || pl.childNodeSize() == 0) return "";
    Elements spans = pl.select("span");
    if (spans.size() < 2) return "";
    Node nmin = spans.get(0);
    Node nsub = spans.get(1);
    String subtype = nsub.attr("style");

    String mins = nmin.childNode(0).toString();
    Integer minp = getMin(name);
    Integer minsub = extractMin(mins);
    Integer rem = 91 - minsub;
    if (rem <= 0) rem = 1;
    if (subtype.contains("red")) {
      min.put(name, minp - rem);
    }
    if (subtype.contains("green")) {
      min.put(name, minp + rem);
    }
    if (spans.size() > 3) {
      deductMin(name, spans.get(2));
    }
    return " <- " + mins;
  }

  private static void deductMin(String name, Node nmin) {
    if (nmin == null || nmin.childNodeSize() == 0) return;
    String mins = nmin.childNode(0).toString();
    Integer minp = getMin(name);
    Integer minsub = extractMin(mins);
    Integer rem = 91 - minsub;
    if (rem <= 0) rem = 1;
      min.put(name, minp - rem);
  }

  private static String extractModifier(Node n, String name, String ev) {
    if (!name.isEmpty()) {
      if (ev.length() == 1)return ev;
      if (n.childNodeSize() < 4) return "'";
      String mode = n.childNode(3).toString();
      if (mode.contains("own")) return "`";
      if (mode.contains("pen")) return "\"";
    }
    return "";
  }

  private static Integer extractMin(String s) {
    if (s==null) return 0;
    if (s.length()==0) return 0;
    String clean = s.replaceAll("\'","").replaceAll(" ","");
    String[] parts = clean.split("\\+");
    if (parts.length > 1) {
      int main = Integer.parseInt(parts[0]);
      int ex = Integer.parseInt(parts[1]);
      if (main == 45) return 45;
      return main+ex;
    }
    Integer m = 0;
    try {
      m = Integer.parseInt(parts[0]);
    } catch (NumberFormatException e) {
      // zero
    }
    return m;
  }

  private static Integer getMin(String s) {
    return min.getOrDefault(s, 0);
  }

  private static void adjustMin() {

  }

  private static String toIso(String dob) {
    if (dob==null) return "0000-00-00";
    String[] parts = dob.split("\\.");
    int year = Integer.parseInt(parts[0]);
    int month = Integer.parseInt(parts[1]);
    int day = Integer.parseInt(parts[2]);
    return String.format("%04d-%02d-%02d", year, month, day);
  }

  private static String trim(String s) {
    return s
        .replaceAll("\n", "")
        .replaceAll("^\\s+","").replaceAll("\\s+$","");
  }
}
