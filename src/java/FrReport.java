import org.jsoup.Jsoup;
import org.jsoup.nodes.Document;
import org.jsoup.nodes.Element;
import org.jsoup.nodes.Node;
import org.jsoup.select.Elements;

import java.io.File;
import java.io.IOException;
import java.text.ParseException;
import java.text.SimpleDateFormat;
import java.util.*;

import static java.lang.Integer.min;
import static java.util.stream.Collectors.joining;

public class FrReport {
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

  private static SimpleDateFormat sdf = new SimpleDateFormat("MMMMM dd, yyyy");
  private static SimpleDateFormat idf = new SimpleDateFormat("yyyy-MM-dd");

  static {
    eSymbol.put("goal", "'");
    eSymbol.put("penalty", "\"");
    eSymbol.put("csc", "`");
    eSymbol.put("yellow_card", "#");
    eSymbol.put("red", "!");
    eSymbol.put("penalty_miss", "/");
  };

  public static void main(String args[]) {
    File input = new File(args[0]);
    Document doc = null;
    try {
      doc = Jsoup.parse(input, "UTF-8", "http://us.soccerway.com/");
    } catch (IOException e) {
      e.printStackTrace();
    }
    Element content = doc.selectFirst(".fullbody");
    extractReport(content);
  }

  private static void extractReport(Element content) {
    Element header = content.selectFirst(".gameheader");
    Element lineup = content.selectFirst("#gamelineup");
    Element brief = content.selectFirst(".gamebrief");
    extractHeader(header);
    extractInfo(brief);
    if (lineup != null) {
      hRoster.clear();
      aRoster.clear();
    } else {
      hScorers.forEach(s -> hRoster.add(s));
      aScorers.forEach(s -> aRoster.add(s));
    }
    extractLineups(lineup);
    //extractSubs(lineup);
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
    for (int k=0; k < 11 - hRoster.size(); k++) hRoster.add("      ");
    for (String s : hRoster) {
      Integer m = 90 + min.getOrDefault(s, 0);
      sb.append(s).append(":").append(m).append(",");
    }
    for (String s : hSubs) {
      Integer m = min.getOrDefault(s, 0);
      sb.append(s).append(":").append(m).append(",");
    }
    for (int i=hSubs.size(); i < 22 - (hRoster.size() + hSubs.size()); i++) {
      sb.append(" :0,");
    }
    sb.append(hCoach).append(",");
    for (int k=0; k < 11 - aRoster.size(); k++) aRoster.add("      ");
    for (String s : aRoster) {
      Integer m = 90 + min.getOrDefault(s, 0);
      sb.append(s).append(":").append(m).append(",");
    }
    for (String s : aSubs) {
      Integer m = min.getOrDefault(s, 0);
      sb.append(s).append(":").append(m).append(",");
    }
    for (int i=aSubs.size(); i < 22 - (aRoster.size() + aSubs.size()); i++) {
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
    if (header==null) return;
    Elements events = header.select(".gametech");
    if (events.isEmpty()) return;
    Element teams = events.get(0);
    Element ehome = teams.selectFirst(".firstteam");
    Element ehtitle = ehome.selectFirst(".title");
    Element ehcoach = ehome.selectFirst(".manager");
    home = extractLink(ehtitle, " ", -2, -1);
    hCoach = extractLink(ehcoach, " ", -1, 0);
    Element escore = teams.selectFirst(".score0");
    score = escore.childNode(0).toString().trim();
    Element eaway = teams.selectFirst(".secondteam");
    Element eatitle = eaway.selectFirst(".title");
    Element eacoach = eaway.selectFirst(".manager");
    away = extractLink(eatitle, " ", -2, -1);
    aCoach = extractLink(eacoach, " ", -1, 0);
    Element goals = events.size() > 1 ? events.get(1) : null;
    extractEvents(goals);
 }

  private static void extractInfo(Element data) {
    if (data==null) return;
    Element ecomp = data.selectFirst("label:contains(Competition)");
    competition = ecomp.nextSibling().childNode(2).childNode(0).toString();
    Element eround = data.selectFirst("label:contains(Round)");
    round = eround.nextElementSibling().childNode(0).childNode(0).toString().replace("Round ","");
    Element edate = data.selectFirst("label:contains(Date)");
    date = edate.nextElementSibling().childNode(0).toString();
    Element evenue = data.selectFirst("label:contains(Stadium)");
    venue = evenue!=null ? evenue.nextElementSibling().childNode(0).toString() : " ";
    Element eattend = data.selectFirst("label:contains(Attendance)");
    attend = eattend!=null? eattend.nextElementSibling().childNode(0).toString().replaceAll(" ", "") : " ";
    Element eref = data.selectFirst("label:contains(Referee)");
    referee = eref!=null ? eref.nextSibling().childNode(1).toString().trim() : " ";
  }

  private static String extractText(Element e, int j) {
    if (e!=null && e.childNodeSize()>0) return e.childNode(j).toString();
    return " ";
  }

  private static String extractLink(Element e, String def, int start, int end) {
    if (e == null) return def;
    Node n = e.selectFirst("a[href]");
    return extractLink(n, def, start, end);
  }

  private static String extractLink(Node n, String def, int start, int end) {
    if (n==null) return def;
    String link = n.attr("href");
    List<String> parts = Arrays.asList(link.split("/"));
    int np = parts.size();
    if (start < 0) {
      return parts.subList(np + start, np + end).stream().collect(joining("/"));
    }
    else {
      return parts.subList(start, end).stream().collect(joining("/"));
    }
  }

  private static String extractDisplayName(Element e) {
    if (e == null) return "";
    Element elink = e.selectFirst("a[href]");
    if (elink==null) return "";
    return elink.childNode(0).toString();
  }

  private static void extractEvents(Element e) {
    if (e==null) return;
    Element alist = e.selectFirst("#actionlist");
    Elements rows = alist.children();
    hRoster.add("      ");
    aRoster.add("      ");
    for (Element row : rows) {
      if (!row.className().equals("line summary")) continue;
      Elements cols = row.children();
      if (cols.size() < 3) continue;
      Element epa = row.selectFirst(".firstteam");
      Element emin = row.selectFirst(".minute");
      Element epb = row.selectFirst(".secondteam");

      String ev = "'";
      String m = emin.childNode(0).toString().trim().replaceAll("'", "");

      String pan = extractLink(epa, "", -1, 0);
      String padn = extractDisplayName(epa);
      String moda = extractEventType(epa);
      String pbn = extractLink(epb, "", -1, 0);
      String pbdn = extractDisplayName(epb);
      String modb = extractEventType(epb);

      if (ev.length() > 0) {
        evol.add(m+ ": " + padn + pbdn + moda + modb);
        if (pbn.isEmpty()) {
          if (moda.length() > 0) {
            if (!moda.equals("`")) hScorers.add(pan);
            else aScorers.add(pan);
          }
        }
        if (pan.isEmpty()) {
          if (modb.length() > 0) {
            if (!modb.equals("`")) aScorers.add(pbn);
            else hScorers.add(pbn);
          }
        }
      }
      if (ev.equals("!")) {
        String name = pan + pbn;
        Integer mins = getMin(name);
        Integer mine = extractMin(m);
        Integer rem = 91 - mine;
        if (rem <= 0) rem = 1;
        if (pan.isEmpty() || pbn.isEmpty()) {
          min.put(name, mins - rem);
        } else {
          mins = getMin(pan);
          min.put(pan, mins - rem);
          mins = getMin(pbn);
          min.put(pbn, mins - rem);
        }
      }
      Integer imin = extractMin(m);
      if (pan.isEmpty() || pbn.isEmpty()) {
        events.add(pan + pbn + moda + modb + imin);
      } else {
        events.add(pan + moda + imin);
        events.add(pbn + modb + imin);
      }
    }
  }

  private static String extractEventType(Element e) {
    if (e == null || e.childNodeSize() == 0) return "";
    Element c = e.child(0);
    if (c==null) return "";
    String ecl = c.attr("class");
    if (ecl==null) return "";
    Element eplus = c.selectFirst(".plusinfo");
    if (eplus!=null) {
      String info = eplus.childNode(0).toString();
      if (info.contains("penalty")) ecl = "penalty";
    }
    String evt = eSymbol.getOrDefault(ecl, "");
    return evt;
  }

  private static void extractLineups(Element e) {
    if (e==null) return;
    Element wrapper = e.childrenSize() > 0 ? e.child(e.childrenSize() -1) : null;
    Element team1 = wrapper != null & wrapper.childrenSize() > 1 ? wrapper.child(1) : null;
    if (team1 == null) return;
    Elements hnames = team1.select(".name");
    for (Element hname : hnames) {
      String n = extractLink(hname, " ", -1, 0);
      if (!n.equals(" ")) hRoster.add(n);
    }
    Element team2 = wrapper != null & wrapper.childrenSize() > 2 ? wrapper.child(2) : null;
    if (team2 == null) return;
    Elements anames = team2.select(".name");
    for (Element aname : anames) {
      String n = extractLink(aname, " ", -1, 0);
      if (!n.equals(" ")) aRoster.add(n);
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
    String name = extractLink(e, " ", -1, -1);
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

  private static String toIso(String s) {
    try {
      Date d = sdf.parse(s);
      String iso = idf.format(d);
      return iso;
    } catch (ParseException e) {
      return "0000-00-00";
    }
  }

  private static String trim(String s) {
    return s
        .replaceAll("\n", "")
        .replaceAll("^\\s+","").replaceAll("\\s+$","");
  }
}
